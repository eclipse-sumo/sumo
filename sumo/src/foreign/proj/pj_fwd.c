/* general forward projection */
#ifndef lint
static const char SCCSID[]="@(#)pj_fwd.c	4.4	93/06/12	GIE	REL";
#endif
#define PJ_LIB__
#include <projects.h>
#include <errno.h>
# define EPS 1.0e-12
	XY /* forward projection entry */
pj_fwd(LP lp, PJ *P) {
	XY xy;
	double t;

	/* check for forward and latitude or longitude overange */
	if ((t = fabs(lp.phi)-HALFPI) > EPS || fabs(lp.lam) > 10.) {
		xy.x = xy.y = HUGE_VAL;
		pj_errno = -14;
	} else { /* proceed with projection */
		errno = pj_errno = 0;
		if (fabs(t) <= EPS)
			lp.phi = lp.phi < 0. ? -HALFPI : HALFPI;
		else if (P->geoc)
			lp.phi = atan(P->rone_es * tan(lp.phi));
		lp.lam -= P->lam0;	/* compute del lp.lam */
		if (!P->over)
			lp.lam = adjlon(lp.lam); /* adjust del longitude */
		xy = (*P->fwd)(lp, P); /* project */
		if (pj_errno || (pj_errno = errno))
			xy.x = xy.y = HUGE_VAL;
		/* adjust for major axis and easting/northings */
		else {
			xy.x = P->fr_meter * (P->a * xy.x + P->x0);
			xy.y = P->fr_meter * (P->a * xy.y + P->y0);
		}
	}
	return xy;
}
