#ifndef lint
static const char SCCSID[]="@(#)PJ_putp2.c	4.1	94/02/15	GIE	REL";
#endif
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(putp2, "Putnins P2") "\n\tPCyl., Sph.";
#define C_x	1.89490
#define C_y	1.71848
#define C_p 0.6141848493043784
#define EPS	1e-10
#define NITER	10
#define PI_DIV_3	1.0471975511965977
FORWARD(s_forward); /* spheroid */
	double p, c, s, V;
	int i;

	p = C_p * sin(lp.phi);
	s = lp.phi * lp.phi;
	lp.phi *= 0.615709 + s * ( 0.00909953 + s * 0.0046292 );
	for (i = NITER; i ; --i) {
		c = cos(lp.phi);
		s = sin(lp.phi);
		lp.phi -= V = (lp.phi + s * (c - 1.) - p) /
			(1. + c * (c - 1.) - s * s);
		if (fabs(V) < EPS)
			break;
	}
	if (!i)
		lp.phi = lp.phi < 0 ? - PI_DIV_3 : PI_DIV_3;
	xy.x = C_x * lp.lam * (cos(lp.phi) - 0.5);
	xy.y = C_y * sin(lp.phi);
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double c;

	lp.phi = aasin(xy.y / C_y);
	lp.lam = xy.x / (C_x * ((c = cos(lp.phi)) - 0.5));
	lp.phi = aasin((lp.phi + sin(lp.phi) * (c - 1.)) / C_p);
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(putp2) P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
