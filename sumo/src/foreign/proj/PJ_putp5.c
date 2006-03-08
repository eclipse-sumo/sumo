#ifndef lint
static const char SCCSID[]="@(#)PJ_putp5.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	A, B;
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(putp5, "Putnins P5") "\n\tPCyl., Sph.";
PROJ_HEAD(putp5p, "Putnins P5'") "\n\tPCyl., Sph.";
#define C	1.01346
#define D	1.2158542
FORWARD(s_forward); /* spheroid */
	xy.x = C * lp.lam * (P->A - P->B * sqrt(1. + D * lp.phi * lp.phi));
	xy.y = C * lp.phi;
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.phi = xy.y / C;
	lp.lam = xy.x / (C * (P->A - P->B * sqrt(1. + D * lp.phi * lp.phi)));
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
	static PJ *
setup(PJ *P) {
	P->es = 0.; P->inv = s_inverse; P->fwd = s_forward;
	return P;
}
ENTRY0(putp5) P->A = 2.; P->B = 1.; ENDENTRY(setup(P))
ENTRY0(putp5p) P->A = 1.5; P->B = 0.5; ENDENTRY(setup(P))
