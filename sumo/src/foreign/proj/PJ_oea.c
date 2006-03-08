#ifndef lint
static const char SCCSID[]="@(#)PJ_oea.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	theta; \
	double	m, n; \
	double	two_r_m, two_r_n, rm, rn, hm, hn; \
	double	cp0, sp0;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(oea, "Oblated Equal Area") "\n\tMisc Sph\n\tn= m= theta=";
FORWARD(s_forward); /* sphere */
	double Az, M, N, cp, sp, cl, shz;

	cp = cos(lp.phi);
	sp = sin(lp.phi);
	cl = cos(lp.lam);
	Az = aatan2(cp * sin(lp.lam), P->cp0 * sp - P->sp0 * cp * cl) + P->theta;
	shz = sin(0.5 * aacos(P->sp0 * sp + P->cp0 * cp * cl));
	M = aasin(shz * sin(Az));
	N = aasin(shz * cos(Az) * cos(M) / cos(M * P->two_r_m));
	xy.y = P->n * sin(N * P->two_r_n);
	xy.x = P->m * sin(M * P->two_r_m) * cos(N) / cos(N * P->two_r_n);
	return (xy);
}
INVERSE(s_inverse); /* sphere */
	double N, M, xp, yp, z, Az, cz, sz, cAz;

	N = P->hn * aasin(xy.y * P->rn);
	M = P->hm * aasin(xy.x * P->rm * cos(N * P->two_r_n) / cos(N));
	xp = 2. * sin(M);
	yp = 2. * sin(N) * cos(M * P->two_r_m) / cos(M);
	cAz = cos(Az = aatan2(xp, yp) - P->theta);
	z = 2. * aasin(0.5 * hypot(xp, yp));
	sz = sin(z);
	cz = cos(z);
	lp.phi = aasin(P->sp0 * cz + P->cp0 * sz * cAz);
	lp.lam = aatan2(sz * sin(Az),
		P->cp0 * cz - P->sp0 * sz * cAz);
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(oea)
	if (((P->n = pj_param(P->params, "dn").f) <= 0.) ||
		((P->m = pj_param(P->params, "dm").f) <= 0.))
		E_ERROR(-39)
	else {
		P->theta = pj_param(P->params, "rtheta").f;
		P->sp0 = sin(P->phi0);
		P->cp0 = cos(P->phi0);
		P->rn = 1./ P->n;
		P->rm = 1./ P->m;
		P->two_r_n = 2. * P->rn;
		P->two_r_m = 2. * P->rm;
		P->hm = 0.5 * P->m;
		P->hn = 0.5 * P->n;
		P->fwd = s_forward;
		P->inv = s_inverse;
		P->es = 0.;
	}
ENDENTRY(P)
