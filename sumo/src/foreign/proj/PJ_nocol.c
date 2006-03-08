#ifndef lint
static const char SCCSID[]="@(#)PJ_nocol.c	4.1	94/02/15	GIE	REL";
#endif
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(nicol, "Nicolosi Globular") "\n\tMisc Sph, no inv.";
#define EPS	1e-10
FORWARD(s_forward); /* spheroid */
	if (fabs(lp.lam) < EPS) {
		xy.x = 0;
		xy.y = lp.phi;
	} else if (fabs(lp.phi) < EPS) {
		xy.x = lp.lam;
		xy.y = 0.;
	} else if (fabs(fabs(lp.lam) - HALFPI) < EPS) {
		xy.x = lp.lam * cos(lp.phi);
		xy.y = HALFPI * sin(lp.phi);
	} else if (fabs(fabs(lp.phi) - HALFPI) < EPS) {
		xy.x = 0;
		xy.y = lp.phi;
	} else {
		double tb, c, d, m, n, r2, sp;

		tb = HALFPI / lp.lam - lp.lam / HALFPI;
		c = lp.phi / HALFPI;
		d = (1 - c * c)/((sp = sin(lp.phi)) - c);
		r2 = tb / d;
		r2 *= r2;
		m = (tb * sp / d - 0.5 * tb)/(1. + r2);
		n = (sp / r2 + 0.5 * d)/(1. + 1./r2);
		xy.x = cos(lp.phi);
		xy.x = sqrt(m * m + xy.x * xy.x / (1. + r2));
		xy.x = HALFPI * ( m + (lp.lam < 0. ? -xy.x : xy.x));
		xy.y = sqrt(n * n - (sp * sp / r2 + d * sp - 1.) /
			(1. + 1./r2));
		xy.y = HALFPI * ( n + (lp.phi < 0. ? xy.y : -xy.y ));
	}
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(nicol) P->es = 0.; P->fwd = s_forward; ENDENTRY(P)
