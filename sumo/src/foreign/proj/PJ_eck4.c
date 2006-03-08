#ifndef lint
static const char SCCSID[]="@(#)PJ_eck4.c	4.1	94/02/15	GIE	REL";
#endif
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(eck4, "Eckert IV") "\n\tPCyl, Sph.";
#define C_x	.42223820031577120149
#define C_y	1.32650042817700232218
#define RC_y	.75386330736002178205
#define C_p	3.57079632679489661922
#define RC_p	.28004957675577868795
#define EPS	1e-7
#define NITER	6
FORWARD(s_forward); /* spheroid */
	double p, V, s, c;
	int i;

	p = C_p * sin(lp.phi);
	V = lp.phi * lp.phi;
	lp.phi *= 0.895168 + V * ( 0.0218849 + V * 0.00826809 );
	for (i = NITER; i ; --i) {
		c = cos(lp.phi);
		s = sin(lp.phi);
		lp.phi -= V = (lp.phi + s * (c + 2.) - p) /
			(1. + c * (c + 2.) - s * s);
		if (fabs(V) < EPS)
			break;
	}
	if (!i) {
		xy.x = C_x * lp.lam;
		xy.y = lp.phi < 0. ? -C_y : C_y;
	} else {
		xy.x = C_x * lp.lam * (1. + cos(lp.phi));
		xy.y = C_y * sin(lp.phi);
	}
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double c;

	lp.phi = aasin(xy.y / C_y);
	lp.lam = xy.x / (C_x * (1. + (c = cos(lp.phi))));
	lp.phi = aasin((lp.phi + sin(lp.phi) * (c + 2.)) / C_p);
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(eck4); P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
