#ifndef lint
static const char SCCSID[]="@(#)PJ_mbtfpp.c	4.1 94/02/15     GIE     REL";
#endif
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(mbtfpp, "McBride-Thomas Flat-Polar Parabolic") "\n\tCyl., Sph.";
#define CS	.95257934441568037152
#define FXC	.92582009977255146156
#define FYC	3.40168025708304504493
#define C23	.66666666666666666666
#define C13	.33333333333333333333
#define ONEEPS	1.0000001
FORWARD(s_forward); /* spheroid */
	lp.phi = asin(CS * sin(lp.phi));
	xy.x = FXC * lp.lam * (2. * cos(C23 * lp.phi) - 1.);
	xy.y = FYC * sin(C13 * lp.phi);
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.phi = xy.y / FYC;
	if (fabs(lp.phi) >= 1.) {
		if (fabs(lp.phi) > ONEEPS)	I_ERROR
		else	lp.phi = (lp.phi < 0.) ? -HALFPI : HALFPI;
	} else
		lp.phi = asin(lp.phi);
	lp.lam = xy.x / ( FXC * (2. * cos(C23 * (lp.phi *= 3.)) - 1.) );
	if (fabs(lp.phi = sin(lp.phi) / CS) >= 1.) {
		if (fabs(lp.phi) > ONEEPS)	I_ERROR
		else	lp.phi = (lp.phi < 0.) ? -HALFPI : HALFPI;
	} else
		lp.phi = asin(lp.phi);
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(mbtfpp) P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
