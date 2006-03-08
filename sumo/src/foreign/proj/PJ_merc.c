#ifndef lint
static const char SCCSID[]="@(#)PJ_merc.c	4.1	94/02/15	GIE	REL";
#endif
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(merc, "Mercator") "\n\tCyl, Sph&Ell\n\tlat_ts=";
#define EPS10 1.e-10
FORWARD(e_forward); /* ellipsoid */
	if (fabs(fabs(lp.phi) - HALFPI) <= EPS10) F_ERROR;
	xy.x = P->k0 * lp.lam;
	xy.y = - P->k0 * log(pj_tsfn(lp.phi, sin(lp.phi), P->e));
	return (xy);
}
FORWARD(s_forward); /* spheroid */
	if (fabs(fabs(lp.phi) - HALFPI) <= EPS10) F_ERROR;
	xy.x = P->k0 * lp.lam;
	xy.y = P->k0 * log(tan(FORTPI + .5 * lp.phi));
	return (xy);
}
INVERSE(e_inverse); /* ellipsoid */
	if ((lp.phi = pj_phi2(exp(- xy.y / P->k0), P->e)) == HUGE_VAL) I_ERROR;
	lp.lam = xy.x / P->k0;
	return (lp);
}
INVERSE(s_inverse); /* spheroid */
	lp.phi = HALFPI - 2. * atan(exp(-xy.y / P->k0));
	lp.lam = xy.x / P->k0;
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(merc)
	double phits=0.0;
	int is_phits;

	if( (is_phits = pj_param(P->params, "tlat_ts").i) ) {
		phits = fabs(pj_param(P->params, "rlat_ts").f);
		if (phits >= HALFPI) E_ERROR(-24);
	}
	if (P->es) { /* ellipsoid */
		if (is_phits)
			P->k0 = pj_msfn(sin(phits), cos(phits), P->es);
		P->inv = e_inverse;
		P->fwd = e_forward;
	} else { /* sphere */
		if (is_phits)
			P->k0 = cos(phits);
		P->inv = s_inverse;
		P->fwd = s_forward;
	}
ENDENTRY(P)
