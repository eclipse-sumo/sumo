#ifndef lint
static const char SCCSID[]="@(#)PJ_tmerc.c	4.2	94/06/02	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	esp; \
	double	ml0; \
	double	*en;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(tmerc, "Transverse Mercator") "\n\tCyl, Sph&Ell";
PROJ_HEAD(utm, "Universal Transverse Mercator (UTM)")
	"\n\tCyl, Sph\n\tzone= south";
#define EPS10	1.e-10
#define aks0	P->esp
#define aks5	P->ml0
#define FC1 1.
#define FC2 .5
#define FC3 .16666666666666666666
#define FC4 .08333333333333333333
#define FC5 .05
#define FC6 .03333333333333333333
#define FC7 .02380952380952380952
#define FC8 .01785714285714285714
FORWARD(e_forward); /* ellipse */
	double al, als, n, cosphi, sinphi, t;

	sinphi = sin(lp.phi); cosphi = cos(lp.phi);
	t = fabs(cosphi) > 1e-10 ? sinphi/cosphi : 0.;
	t *= t;
	al = cosphi * lp.lam;
	als = al * al;
	al /= sqrt(1. - P->es * sinphi * sinphi);
	n = P->esp * cosphi * cosphi;
	xy.x = P->k0 * al * (FC1 +
		FC3 * als * (1. - t + n +
		FC5 * als * (5. + t * (t - 18.) + n * (14. - 58. * t)
		+ FC7 * als * (61. + t * ( t * (179. - t) - 479. ) )
		)));
	xy.y = P->k0 * (pj_mlfn(lp.phi, sinphi, cosphi, P->en) - P->ml0 +
		sinphi * al * lp.lam * FC2 * ( 1. +
		FC4 * als * (5. - t + n * (9. + 4. * n) +
		FC6 * als * (61. + t * (t - 58.) + n * (270. - 330 * t)
		+ FC8 * als * (1385. + t * ( t * (543. - t) - 3111.) )
		))));
	return (xy);
}
FORWARD(s_forward); /* sphere */
	double b, cosphi;

	b = (cosphi = cos(lp.phi)) * sin(lp.lam);
	if (fabs(fabs(b) - 1.) <= EPS10) F_ERROR;
	xy.x = aks5 * log((1. + b) / (1. - b));
	if ((b = fabs( xy.y = cosphi * cos(lp.lam) / sqrt(1. - b * b) )) >= 1.) {
		if ((b - 1.) > EPS10) F_ERROR
		else xy.y = 0.;
	} else
		xy.y = acos(xy.y);
	if (lp.phi < 0.) xy.y = -xy.y;
	xy.y = aks0 * (xy.y - P->phi0);
	return (xy);
}
INVERSE(e_inverse); /* ellipsoid */
	double n, con, cosphi, d, ds, sinphi, t;

	lp.phi = pj_inv_mlfn(P->ml0 + xy.y / P->k0, P->es, P->en);
	if (fabs(lp.phi) >= HALFPI) {
		lp.phi = xy.y < 0. ? -HALFPI : HALFPI;
		lp.lam = 0.;
	} else {
		sinphi = sin(lp.phi);
		cosphi = cos(lp.phi);
		t = fabs(cosphi) > 1e-10 ? sinphi/cosphi : 0.;
		n = P->esp * cosphi * cosphi;
		d = xy.x * sqrt(con = 1. - P->es * sinphi * sinphi) / P->k0;
		con *= t;
		t *= t;
		ds = d * d;
		lp.phi -= (con * ds / (1.-P->es)) * FC2 * (1. -
			ds * FC4 * (5. + t * (3. - 9. *  n) + n * (1. - 4 * n) -
			ds * FC6 * (61. + t * (90. - 252. * n +
				45. * t) + 46. * n
		   - ds * FC8 * (1385. + t * (3633. + t * (4095. + 1574. * t)) )
			)));
		lp.lam = d*(FC1 -
			ds*FC3*( 1. + 2.*t + n -
			ds*FC5*(5. + t*(28. + 24.*t + 8.*n) + 6.*n
		   - ds * FC7 * (61. + t * (662. + t * (1320. + 720. * t)) )
		))) / cosphi;
	}
	return (lp);
}
INVERSE(s_inverse); /* sphere */
	double h, g;

	h = exp(xy.x / aks0);
	g = .5 * (h - 1. / h);
	h = cos(P->phi0 + xy.y / aks0);
	lp.phi = asin(sqrt((1. - h * h) / (1. + g * g)));
	if (xy.y < 0.) lp.phi = -lp.phi;
	lp.lam = (g || h) ? atan2(g, h) : 0.;
	return (lp);
}
FREEUP;
	if (P) {
		if (P->en)
			pj_dalloc(P->en);
		pj_dalloc(P);
	}
}
	static PJ *
setup(PJ *P) { /* general initialization */
	if (P->es) {
		if (!(P->en = pj_enfn(P->es)))
			E_ERROR_0;
		P->ml0 = pj_mlfn(P->phi0, sin(P->phi0), cos(P->phi0), P->en);
		P->esp = P->es / (1. - P->es);
		P->inv = e_inverse;
		P->fwd = e_forward;
	} else {
		aks0 = P->k0;
		aks5 = .5 * aks0;
		P->inv = s_inverse;
		P->fwd = s_forward;
	}
	return P;
}
ENTRY1(tmerc, en)
ENDENTRY(setup(P))
ENTRY1(utm, en)
	int zone;

	if (!P->es) E_ERROR(-34);
	P->y0 = pj_param(P->params, "bsouth").i ? 10000000. : 0.;
	P->x0 = 500000.;
	if (pj_param(P->params, "tzone").i) /* zone input ? */
		if ((zone = pj_param(P->params, "izone").i) > 0 && zone <= 60)
			--zone;
		else
			E_ERROR(-35)
	else /* nearest central meridian input */
		if ((zone = floor((adjlon(P->lam0) + PI) * 30. / PI)) < 0)
			zone = 0;
		else if (zone >= 60)
			zone = 59;
	P->lam0 = (zone + .5) * PI / 30. - PI;
	P->k0 = 0.9996;
	P->phi0 = 0.;
ENDENTRY(setup(P))
