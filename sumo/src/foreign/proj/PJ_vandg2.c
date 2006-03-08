#ifndef lint
static const char SCCSID[]="@(#)PJ_vandg2.c	4.1	94/02/15	GIE	REL";
#endif
# define TOL	1e-10
# define TWORPI	0.63661977236758134308
#define PROJ_PARMS__ \
	int	vdg3;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(vandg2, "van der Grinten II") "\n\tMisc Sph, no inv.";
PROJ_HEAD(vandg3, "van der Grinten III") "\n\tMisc Sph, no inv.";
FORWARD(s_forward); /* spheroid */
	double x1, at, bt, ct;

	bt = fabs(TWORPI * lp.phi);
	if ((ct = 1. - bt * bt) < 0.)
		ct = 0.;
	else
		ct = sqrt(ct);
	if (fabs(lp.lam) < TOL) {
		xy.x = 0.;
		xy.y = PI * (lp.phi < 0. ? -bt : bt) / (1. + ct);
	} else {
		at = 0.5 * fabs(PI / lp.lam - lp.lam / PI);
		if (P->vdg3) {
			x1 = bt / (1. + ct);
			xy.x = PI * (sqrt(at * at + 1. - x1 * x1) - at);
			xy.y = PI * x1;
		} else {
			x1 = (ct * sqrt(1. + at * at) - at * ct * ct) /
				(1. + at * at * bt * bt);
			xy.x = PI * x1;
			xy.y = PI * sqrt(1. - x1 * (x1 + 2. * at) + TOL);
		}
		if ( lp.lam < 0.) xy.x = -xy.x;
		if ( lp.phi < 0.) xy.y = -xy.y;
	}
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(vandg2)
	P->vdg3 = 0;
	P->inv = 0; P->fwd = s_forward;
ENDENTRY(P)
ENTRY0(vandg3)
	P->vdg3 = 1;
	P->es = 0.; P->fwd = s_forward;
ENDENTRY(P)
