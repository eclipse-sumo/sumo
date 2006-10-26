/* allocate and deallocate memory */
#ifndef lint
static const char SCCSID[]="@(#)pj_malloc.c	4.3   93/06/12 GIE REL";
#endif
/* These routines are used so that applications can readily replace
** projection system memory allocation/deallocation call with custom
** application procedures.  */
#include <projects.h>
#include <errno.h>

	void *
pj_malloc(size_t size) {
// Currently, pj_malloc is a hack to solve an errno problem.
// The problem is described in more details at
// https://bugzilla.redhat.com/bugzilla/show_bug.cgi?id=86420.
// It seems, that pj_init and similar functions incorrectly
// (under debian/glibs-2.3.2) assume that pj_malloc resets
// errno after success. pj_malloc tries to mimic this.
        int old_errno = errno;
        void *res = malloc(size);
        if ( res && !old_errno )
                errno = 0;
        return res;
}
	void
pj_dalloc(void *ptr) {
	free(ptr);
}
