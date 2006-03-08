#ifndef lint
static const char SCCSID[]="@(#)PJ_wink2.c	4.1 94/02/15     GIE     REL";
#endif
#define PROJ_PARMS__ \
	double	cosphi1;
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(wink2, "Winkel II") "\n\tPCyl., Sph., no inv.\n\tlat_1=";
#define MAX_ITER    10
#define LOOP_TOL    1e-7
#define TWO_D_PI 0.636619772367581343
FORWARD(s_forward); /* spheroid */
	double k, V;
	int i;

	xy.y = lp.phi * TWO_D_PI;
	k = PI * sin(lp.phi);
	lp.phi *= 1.8;
	for (i = MAX_ITER; i ; --i) {
		lp.phi -= V = (lp.phi + sin(lp.phi) - k) /
			(1. + cos(lp.phi));
		if (fabs(V) < LOOP_TOL)
			break;
	}
	if (!i)
		lp.phi = (lp.phi < 0.) ? -HALFPI : HALFPI;
	else
		lp.phi *= 0.5;
	xy.x = 0.5 * lp.lam * (cos(lp.phi) + P->cosphi1);
	xy.y = FORTPI * (sin(lp.phi) + xy.y);
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(wink2)
	P->cosphi1 = cos(pj_param(P->params, "rlat_1").f);
	P->es = 0.; P->inv = 0; P->fwd = s_forward;
ENDENTRY(P)
