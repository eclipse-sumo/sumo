#ifndef lint
static const char SCCSID[]="@(#)PJ_cea.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double qp; \
	double *apa;
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(cea, "Equal Area Cylindrical") "\n\tCyl, Sph&Ell\n\tlat_ts=";
# define EPS	1e-10
FORWARD(e_forward); /* spheroid */
	xy.x = P->k0 * lp.lam;
	xy.y = .5 * pj_qsfn(sin(lp.phi), P->e, P->one_es) / P->k0;
	return (xy);
}
FORWARD(s_forward); /* spheroid */
	xy.x = P->k0 * lp.lam;
	xy.y = sin(lp.phi) / P->k0;
	return (xy);
}
INVERSE(e_inverse); /* spheroid */
	lp.phi = pj_authlat(asin( 2. * xy.y * P->k0 / P->qp), P->apa);
	lp.lam = xy.x / P->k0;
	return (lp);
}
INVERSE(s_inverse); /* spheroid */
	double t;

	if ((t = fabs(xy.y *= P->k0)) - EPS <= 1.) {
		if (t >= 1.)
			lp.phi = xy.y < 0. ? -HALFPI : HALFPI;
		else
			lp.phi = asin(xy.y);
		lp.lam = xy.x / P->k0;
	} else I_ERROR;
	return (lp);
}
FREEUP;
	if (P) {
		if (P->apa)
			pj_dalloc(P->apa);
		pj_dalloc(P);
	}
}
ENTRY1(cea, apa)
	double t;

	if (pj_param(P->params, "tlat_ts").i &&
		(P->k0 = cos(t = pj_param(P->params, "rlat_ts").f)) < 0.) E_ERROR(-24)
	else
		t = 0.;
	if (P->es) {
		t = sin(t);
		P->k0 /= sqrt(1. - P->es * t * t);
		P->e = sqrt(P->es);
		if (!(P->apa = pj_authset(P->es))) E_ERROR_0;
		P->qp = pj_qsfn(1., P->e, P->one_es);
		P->inv = e_inverse;
		P->fwd = e_forward;
	} else {
		P->inv = s_inverse;
		P->fwd = s_forward;
	}
ENDENTRY(P)
