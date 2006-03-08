#ifndef lint
static const char SCCSID[]="@(#)PJ_putp6.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double C_x, C_y, A, B, D;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(putp6, "Putnins P6") "\n\tPCyl., Sph.";
PROJ_HEAD(putp6p, "Putnins P6'") "\n\tPCyl., Sph.";
#define EPS	1e-10
#define NITER	10
#define CON_POLE 1.732050807568877
FORWARD(s_forward); /* spheroid */
	double p, r, V;
	int i;

	p = P->B * sin(lp.phi);
	lp.phi *=  1.10265779;
	for (i = NITER; i ; --i) {
		r = sqrt(1. + lp.phi * lp.phi);
		lp.phi -= V = ( (P->A - r) * lp.phi - log(lp.phi + r) - p ) /
			(P->A - 2. * r);
		if (fabs(V) < EPS)
			break;
	}
	if (!i)
		lp.phi = p < 0. ? -CON_POLE : CON_POLE;
	xy.x = P->C_x * lp.lam * (P->D - sqrt(1. + lp.phi * lp.phi));
	xy.y = P->C_y * lp.phi;
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double r;

	lp.phi = xy.y / P->C_y;
	r = sqrt(1. + lp.phi * lp.phi);
	lp.lam = xy.x / (P->C_x * (P->D - r));
	lp.phi = aasin( ( (P->A - r) * lp.phi - log(lp.phi + r) ) / P->B);
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
	static PJ *
setup(PJ *P) {
	P->es = 0.;
	P->inv = s_inverse;
	P->fwd = s_forward;
	return P;
}
ENTRY0(putp6)
	P->C_x = 1.01346;
	P->C_y = 0.91910;
	P->A   = 4.;
	P->B   = 2.1471437182129378784;
	P->D   = 2.;
ENDENTRY(setup(P))
ENTRY0(putp6p)
	P->C_x = 0.44329;
	P->C_y = 0.80404;
	P->A   = 6.;
	P->B   = 5.61125;
	P->D   = 3.;
ENDENTRY(setup(P))
