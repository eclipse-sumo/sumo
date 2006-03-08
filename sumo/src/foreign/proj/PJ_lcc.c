#ifndef lint
static const char SCCSID[]="@(#)PJ_lcc.c	4.2	94/03/18	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	phi1; \
	double	phi2; \
	double	n; \
	double	rho; \
	double	rho0; \
	double	c; \
	int		ellips;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(lcc, "Lambert Conformal Conic")
	"\n\tConic, Sph&Ell\n\tlat_1= and lat_2= or lat_0";
# define EPS10	1.e-10
FORWARD(e_forward); /* ellipsoid & spheroid */
	if (fabs(fabs(lp.phi) - HALFPI) < EPS10) {
		if ((lp.phi * P->n) <= 0.) F_ERROR;
		P->rho = 0.;
		}
	else
		P->rho = P->c * (P->ellips ? pow(pj_tsfn(lp.phi, sin(lp.phi),
			P->e), P->n) : pow(tan(FORTPI + .5 * lp.phi), -P->n));
	xy.x = P->k0 * (P->rho * sin( lp.lam *= P->n ) );
	xy.y = P->k0 * (P->rho0 - P->rho * cos(lp.lam) );
	return (xy);
}
INVERSE(e_inverse); /* ellipsoid & spheroid */
	xy.x /= P->k0;
	xy.y /= P->k0;
	if( (P->rho = hypot(xy.x, xy.y = P->rho0 - xy.y)) != 0.0) {
		if (P->n < 0.) {
			P->rho = -P->rho;
			xy.x = -xy.x;
			xy.y = -xy.y;
		}
		if (P->ellips) {
			if ((lp.phi = pj_phi2(pow(P->rho / P->c, 1./P->n), P->e))
				== HUGE_VAL)
				I_ERROR;
		} else
			lp.phi = 2. * atan(pow(P->c / P->rho, 1./P->n)) - HALFPI;
		lp.lam = atan2(xy.x, xy.y) / P->n;
	} else {
		lp.lam = 0.;
		lp.phi = P->n > 0. ? HALFPI : - HALFPI;
	}
	return (lp);
}
SPECIAL(fac) {
	if (fabs(fabs(lp.phi) - HALFPI) < EPS10) {
		if ((lp.phi * P->n) <= 0.) return;
		P->rho = 0.;
	} else
		P->rho = P->c * (P->ellips ? pow(pj_tsfn(lp.phi, sin(lp.phi),
			P->e), P->n) : pow(tan(FORTPI + .5 * lp.phi), -P->n));
	fac->code |= IS_ANAL_HK + IS_ANAL_CONV;
	fac->k = fac->h = P->k0 * P->n * P->rho /
		pj_msfn(sin(lp.phi), cos(lp.phi), P->es);
	fac->conv = - P->n * lp.lam;
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(lcc)
	double cosphi, sinphi;
	int secant;

	P->phi1 = pj_param(P->params, "rlat_1").f;
	if (pj_param(P->params, "tlat_2").i)
		P->phi2 = pj_param(P->params, "rlat_2").f;
	else {
		P->phi2 = P->phi1;
		if (!pj_param(P->params, "tlat_0").i)
			P->phi0 = P->phi1;
	}
	if (fabs(P->phi1 + P->phi2) < EPS10) E_ERROR(-21);
	P->n = sinphi = sin(P->phi1);
	cosphi = cos(P->phi1);
	secant = fabs(P->phi1 - P->phi2) >= EPS10;
	if( (P->ellips = (P->es != 0.)) ) {
		double ml1, m1;

		P->e = sqrt(P->es);
		m1 = pj_msfn(sinphi, cosphi, P->es);
		ml1 = pj_tsfn(P->phi1, sinphi, P->e);
		if (secant) { /* secant cone */
			P->n = log(m1 /
			   pj_msfn(sinphi = sin(P->phi2), cos(P->phi2), P->es));
			P->n /= log(ml1 / pj_tsfn(P->phi2, sinphi, P->e));
		}
		P->c = (P->rho0 = m1 * pow(ml1, -P->n) / P->n);
		P->rho0 *= (fabs(fabs(P->phi0) - HALFPI) < EPS10) ? 0. :
			pow(pj_tsfn(P->phi0, sin(P->phi0), P->e), P->n);
	} else {
		if (secant)
			P->n = log(cosphi / cos(P->phi2)) /
			   log(tan(FORTPI + .5 * P->phi2) /
			   tan(FORTPI + .5 * P->phi1));
		P->c = cosphi * pow(tan(FORTPI + .5 * P->phi1), P->n) / P->n;
		P->rho0 = (fabs(fabs(P->phi0) - HALFPI) < EPS10) ? 0. :
			P->c * pow(tan(FORTPI + .5 * P->phi0), -P->n);
	}
	P->inv = e_inverse;
	P->fwd = e_forward;
	P->spc = fac;
ENDENTRY(P)
