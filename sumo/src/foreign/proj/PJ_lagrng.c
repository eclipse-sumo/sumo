#ifndef lint
static const char SCCSID[]="@(#)PJ_lagrng.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	hrw; \
	double	rw; \
	double	a1;
#define TOL	1e-10
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(lagrng, "Lagrange") "\n\tMisc Sph, no inv.\n\tW=";
FORWARD(s_forward); /* spheroid */
	double v, c;

	if (fabs(fabs(lp.phi) - HALFPI) < TOL) {
		xy.x = 0;
		xy.y = lp.phi < 0 ? -2. : 2.;
	} else {
		lp.phi = sin(lp.phi);
		v = P->a1 * pow((1. + lp.phi)/(1. - lp.phi), P->hrw);
		if ((c = 0.5 * (v + 1./v) + cos(lp.lam *= P->rw)) < TOL)
			F_ERROR;
		xy.x = 2. * sin(lp.lam) / c;
		xy.y = (v - 1./v) / c;
	}
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(lagrng)
	double phi1;

	if ((P->rw = pj_param(P->params, "dW").f) <= 0) E_ERROR(-27);
	P->hrw = 0.5 * (P->rw = 1. / P->rw);
	phi1 = pj_param(P->params, "rlat_1").f;
	if (fabs(fabs(phi1 = sin(phi1)) - 1.) < TOL) E_ERROR(-22);
	P->a1 = pow((1. - phi1)/(1. + phi1), P->hrw);
	P->es = 0.; P->fwd = s_forward;
ENDENTRY(P)
