#ifndef lint
static const char SCCSID[]="@(#)PJ_wag7.c	4.1	94/02/15	GIE	REL";
#endif
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(wag7, "Wagner VII") "\n\tMisc Sph, no inv.";
FORWARD(s_forward); /* sphere */
	double theta, ct, D;

	theta = asin(xy.y = 0.90630778703664996 * sin(lp.phi));
	xy.x = 2.66723 * (ct = cos(theta)) * sin(lp.lam /= 3.);
	xy.y *= 1.24104 * (D = 1/(sqrt(0.5 * (1 + ct * cos(lp.lam)))));
	xy.x *= D;
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(wag7) P->fwd = s_forward; P->inv = 0; P->es = 0.; ENDENTRY(P)
