#ifndef lint
static const char SCCSID[]="@(#)PJ_tcc.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double ap;
#define EPS10 1.e-10
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(tcc, "Transverse Central Cylindrical") "\n\tCyl, Sph, no inv.";
FORWARD(s_forward); /* spheroid */
	double b, bt;

	b = cos(lp.phi) * sin(lp.lam);
	if ((bt = 1. - b * b) < EPS10) F_ERROR;
	xy.x = b / sqrt(bt);
	xy.y = atan2(tan(lp.phi) , cos(lp.lam));
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(tcc) P->es = 0.; P->fwd = s_forward; ENDENTRY(P)
