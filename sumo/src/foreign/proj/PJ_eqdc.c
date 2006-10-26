#ifndef lint
static const char SCCSID[]="@(#)PJ_eqdc.c	4.2	94/03/16	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double phi1; \
	double phi2; \
	double n; \
	double rho; \
	double rho0; \
	double c; \
	double *en; \
	int		ellips;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(eqdc, "Equidistant Conic")
	"\n\tConic, Sph&Ell\n\tlat_1= lat_2=";
# define EPS10	1.e-10
FORWARD(e_forward); /* sphere & ellipsoid */
	P->rho = P->c - (P->ellips ? pj_mlfn(lp.phi, sin(lp.phi),
		cos(lp.phi), P->en) : lp.phi);
	xy.x = P->rho * sin( lp.lam *= P->n );
	xy.y = P->rho0 - P->rho * cos(lp.lam);
	return (xy);
}
INVERSE(e_inverse); /* sphere & ellipsoid */
	if ((P->rho = hypot(xy.x, xy.y = P->rho0 - xy.y)) != 0.0 ) {
		if (P->n < 0.) {
			P->rho = -P->rho;
			xy.x = -xy.x;
			xy.y = -xy.y;
		}
		lp.phi = P->c - P->rho;
		if (P->ellips)
			lp.phi = pj_inv_mlfn(lp.phi, P->es, P->en);
		lp.lam = atan2(xy.x, xy.y) / P->n;
	} else {
		lp.lam = 0.;
		lp.phi = P->n > 0. ? HALFPI : - HALFPI;
	}
	return (lp);
}
SPECIAL(fac) {
	double sinphi, cosphi;

	sinphi = sin(lp.phi);
	cosphi = cos(lp.phi);
	fac->code |= IS_ANAL_HK;
	fac->h = 1.;
	fac->k = P->n * (P->c - (P->ellips ? pj_mlfn(lp.phi, sinphi,
		cosphi, P->en) : lp.phi)) / pj_msfn(sinphi, cosphi, P->es);
}
FREEUP; if (P) { if (P->en) pj_dalloc(P->en); pj_dalloc(P); } }
ENTRY1(eqdc, en)
	double cosphi, sinphi;
	int secant;

	P->phi1 = pj_param(P->params, "rlat_1").f;
	P->phi2 = pj_param(P->params, "rlat_2").f;
	if (fabs(P->phi1 + P->phi2) < EPS10) E_ERROR(-21);
	if (!(P->en = pj_enfn(P->es)))
		E_ERROR_0;
	P->n = sinphi = sin(P->phi1);
	cosphi = cos(P->phi1);
	secant = fabs(P->phi1 - P->phi2) >= EPS10;
	if( (P->ellips = (P->es > 0.)) ) {
		double ml1, m1;

		m1 = pj_msfn(sinphi, cosphi, P->es);
		ml1 = pj_mlfn(P->phi1, sinphi, cosphi, P->en);
		if (secant) { /* secant cone */
			sinphi = sin(P->phi2);
			cosphi = cos(P->phi2);
			P->n = (m1 - pj_msfn(sinphi, cosphi, P->es)) /
				(pj_mlfn(P->phi2, sinphi, cosphi, P->en) - ml1);
		}
		P->c = ml1 + m1 / P->n;
		P->rho0 = P->c - pj_mlfn(P->phi0, sin(P->phi0),
			cos(P->phi0), P->en);
	} else {
		if (secant)
			P->n = (cosphi - cos(P->phi2)) / (P->phi2 - P->phi1);
		P->c = P->phi1 + cos(P->phi1) / P->n;
		P->rho0 = P->c - P->phi0;
	}
	P->inv = e_inverse;
	P->fwd = e_forward;
	P->spc = fac;
ENDENTRY(P)
