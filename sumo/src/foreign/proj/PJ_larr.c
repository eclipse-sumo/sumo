#ifndef lint
static const char SCCSID[]="@(#)PJ_larr.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(larr, "Larrivee") "\n\tMisc Sph, no inv.";
#define SIXTH .16666666666666666
FORWARD(s_forward); /* sphere */
	xy.x = 0.5 * lp.lam * (1. + sqrt(cos(lp.phi)));
	xy.y = lp.phi / (cos(0.5 * lp.phi) * cos(SIXTH * lp.lam));
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(larr) P->fwd = s_forward; P->inv = 0; P->es = 0.; ENDENTRY(P)
