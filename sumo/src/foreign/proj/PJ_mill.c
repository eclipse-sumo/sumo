#ifndef lint
static const char SCCSID[]="@(#)PJ_mill.c	4.1	94/02/15	GIE	REL";
#endif
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(mill, "Miller Cylindrical") "\n\tCyl, Sph";
FORWARD(s_forward); /* spheroid */
	xy.x = lp.lam;
	xy.y = log(tan(FORTPI + lp.phi * .4)) * 1.25;
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.lam = xy.x;
	lp.phi = 2.5 * (atan(exp(.8 * xy.y)) - FORTPI);
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(mill) P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
