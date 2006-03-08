#ifndef lint
static const char SCCSID[]="@(#)PJ_urmfps.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	n, C_y;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(urmfps, "Urmaev Flat-Polar Sinusoidal") "\n\tPCyl, Sph.\n\tn=";
PROJ_HEAD(wag1, "Wagner I (Kavraisky VI)") "\n\tPCyl, Sph.";
#define C_x 0.8773826753
#define Cy 1.139753528477
FORWARD(s_forward); /* sphere */
	lp.phi = aasin(P->n * sin(lp.phi));
	xy.x = C_x * lp.lam * cos(lp.phi);
	xy.y = P->C_y * lp.phi;
	return (xy);
}
INVERSE(s_inverse); /* sphere */
	xy.y /= P->C_y;
	lp.phi = aasin(sin(xy.y) / P->n);
	lp.lam = xy.x / (C_x * cos(xy.y));
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
	static PJ *
setup(PJ *P) {
	P->C_y = Cy / P->n;
	P->es = 0.;
	P->inv = s_inverse;
	P->fwd = s_forward;
	return P;
}
ENTRY0(urmfps)
	if (pj_param(P->params, "tn").i) {
		P->n = pj_param(P->params, "dn").f;
		if (P->n <= 0. || P->n > 1.)
			E_ERROR(-40)
	} else
		E_ERROR(-40)
ENDENTRY(setup(P))
ENTRY0(wag1)
	P->n = 0.8660254037844386467637231707;
ENDENTRY(setup(P))
