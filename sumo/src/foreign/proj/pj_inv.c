/* general inverse projection */
#ifndef lint
static const char SCCSID[]="@(#)pj_inv.c	4.5	93/06/12	GIE	REL";
#endif
#define PJ_LIB__
#include <projects.h>
#include <errno.h>
# define EPS 1.0e-12
	LP /* inverse projection entry */
pj_inv(XY xy, PJ *P) {
	LP lp;

	/* can't do as much preliminary checking as with forward */
	if (xy.x == HUGE_VAL || xy.y == HUGE_VAL) {
		lp.lam = lp.phi = HUGE_VAL;
		pj_errno = -15;
	}
	errno = pj_errno = 0;
	xy.x = (xy.x * P->to_meter - P->x0) * P->ra; /* descale and de-offset */
	xy.y = (xy.y * P->to_meter - P->y0) * P->ra;
	lp = (*P->inv)(xy, P); /* inverse project */
	if (pj_errno || (pj_errno = errno))
		lp.lam = lp.phi = HUGE_VAL;
	else {
		lp.lam += P->lam0; /* reduce from del lp.lam */
		if (!P->over)
			lp.lam = adjlon(lp.lam); /* adjust longitude to CM */
		if (P->geoc && fabs(fabs(lp.phi)-HALFPI) > EPS)
			lp.phi = atan(P->one_es * tan(lp.phi));
	}
	return lp;
}
