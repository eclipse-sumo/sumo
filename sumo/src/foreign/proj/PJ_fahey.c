#ifndef lint
static const char SCCSID[]="@(#)PJ_fahey.c	4.1	94/02/15	GIE	REL";
#endif
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(fahey, "Fahey") "\n\tPcyl, Sph.";
#define TOL 1e-6
FORWARD(s_forward); /* spheroid */
	xy.y = 1.819152 * ( xy.x = tan(0.5 * lp.phi) );
	xy.x = 0.819152 * lp.lam * asqrt(1 - xy.x * xy.x);
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.phi = 2. * atan(xy.y /= 1.819152);
	lp.lam = fabs(xy.y = 1. - xy.y * xy.y) < TOL ? 0. :
		xy.x / (0.819152 * sqrt(xy.y));
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(fahey) P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
