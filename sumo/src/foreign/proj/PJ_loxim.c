#ifndef lint
static const char SCCSID[]="@(#)PJ_loxim.c	4.1 94/02/15     GIE     REL";
#endif
#define PROJ_PARMS__ \
	double phi1; \
	double cosphi1; \
	double tanphi1;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(loxim, "Loximuthal") "\n\tPCyl Sph";
#define EPS	1e-8
FORWARD(s_forward); /* spheroid */
	xy.y = lp.phi - P->phi1;
	if (fabs(xy.y) < EPS)
		xy.x = lp.lam * P->cosphi1;
	else {
		xy.x = FORTPI + 0.5 * lp.phi;
		if (fabs(xy.x) < EPS || fabs(fabs(xy.x) - HALFPI) < EPS)
			xy.x = 0.;
		else
			xy.x = lp.lam * xy.y / log( tan(xy.x) / P->tanphi1 );
	}
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.phi = xy.y + P->phi1;
	if (fabs(xy.y) < EPS)
		lp.lam = xy.x / P->cosphi1;
	else
		if (fabs( lp.lam = FORTPI + 0.5 * lp.phi ) < EPS ||
			fabs(fabs(lp.lam) - HALFPI) < EPS)
			lp.lam = 0.;
		else
			lp.lam = xy.x * log( tan(lp.lam) / P->tanphi1 ) / xy.y ;
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(loxim);
	P->phi1 = pj_param(P->params, "rlat_1").f;
	if ((P->cosphi1 = cos(P->phi1)) < EPS) E_ERROR(-22);
	P->tanphi1 = tan(FORTPI + 0.5 * P->phi1);
	P->inv = s_inverse; P->fwd = s_forward;
	P->es = 0.;
ENDENTRY(P)
