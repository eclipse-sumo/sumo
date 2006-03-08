#ifndef lint
static const char SCCSID[]="@(#)PJ_nell_h.c	4.1	94/02/15	GIE	REL";
#endif
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(nell_h, "Nell-Hammer") "\n\tPCyl., Sph.";
#define NITER 9
#define EPS 1e-7
FORWARD(s_forward); /* spheroid */
	xy.x = 0.5 * lp.lam * (1. + cos(lp.phi));
	xy.y = 2.0 * (lp.phi - tan(0.5 *lp.phi));
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double V, c, p;
	int i;

	p = 0.5 * xy.y;
	for (i = NITER; i ; --i) {
		c = cos(0.5 * lp.phi);
		lp.phi -= V = (lp.phi - tan(lp.phi/2) - p)/(1. - 0.5/(c*c));
		if (fabs(V) < EPS)
			break;
	}
	if (!i) {
		lp.phi = p < 0. ? -HALFPI : HALFPI;
		lp.lam = 2. * xy.x;
	} else
		lp.lam = 2. * xy.x / (1. + cos(lp.phi));
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(nell_h) P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
