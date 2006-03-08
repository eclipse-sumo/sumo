#ifndef lint
static const char SCCSID[]="@(#)PJ_tcea.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double rk0;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(tcea, "Transverse Cylindrical Equal Area") "\n\tCyl, Sph";
FORWARD(s_forward); /* spheroid */
	xy.x = P->rk0 * cos(lp.phi) * sin(lp.lam);
	xy.y = P->k0 * (atan2(tan(lp.phi), cos(lp.lam)) - P->phi0);
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double t;

	xy.y = xy.y * P->rk0 + P->phi0;
	xy.x *= P->k0;
	t = sqrt(1. - xy.x * xy.x);
	lp.phi = asin(t * sin(xy.y));
	lp.lam = atan2(xy.x, t * cos(xy.y));
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(tcea)
	P->rk0 = 1 / P->k0;
	P->inv = s_inverse;
	P->fwd = s_forward;
	P->es = 0.;
ENDENTRY(P)
