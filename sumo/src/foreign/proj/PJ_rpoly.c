#ifndef lint
static const char SCCSID[]="@(#)PJ_rpoly.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	phi1; \
	double	fxa; \
	double	fxb; \
	int		mode;
#define EPS	1e-9
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(rpoly, "Rectangular Polyconic")
	"\n\tConic, Sph., no inv.\n\tlat_ts=";
FORWARD(s_forward); /* spheroid */
	double fa;

	if (P->mode)
		fa = tan(lp.lam * P->fxb) * P->fxa;
	else
		fa = 0.5 * lp.lam;
	if (fabs(lp.phi) < EPS) {
		xy.x = fa + fa;
		xy.y = - P->phi0;
	} else {
		xy.y = 1. / tan(lp.phi);
		xy.x = sin(fa = 2. * atan(fa * sin(lp.phi))) * xy.y;
		xy.y = lp.phi - P->phi0 + (1. - cos(fa)) * xy.y;
	}
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(rpoly)
	if ((P->mode = (P->phi1 = fabs(pj_param(P->params, "rlat_ts").f)) > EPS)) {
		P->fxb = 0.5 * sin(P->phi1);
		P->fxa = 0.5 / P->fxb;
	}
	P->es = 0.; P->fwd = s_forward;
ENDENTRY(P)
