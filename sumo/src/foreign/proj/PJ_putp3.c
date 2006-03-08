#ifndef lint
static const char SCCSID[]="@(#)PJ_putp3.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	A;
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(putp3, "Putnins P3") "\n\tPCyl., Sph.";
PROJ_HEAD(putp3p, "Putnins P3'") "\n\tPCyl., no inv., Sph.";
#define C	0.79788456
#define RPISQ	0.1013211836
FORWARD(s_forward); /* spheroid */
	xy.x = C * lp.lam * (1. - P->A * lp.phi * lp.phi);
	xy.y = C * lp.phi;
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.phi = xy.y / C;
	lp.lam = xy.x / (C * (1. - P->A * lp.phi * lp.phi));
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
	static PJ *
setup(PJ *P) {
	P->es = 0.; P->inv = s_inverse; P->fwd = s_forward;
	return P;
}
ENTRY0(putp3)  P->A = 4. * RPISQ; ENDENTRY(setup(P))
ENTRY0(putp3p) P->A = 2. * RPISQ; ENDENTRY(setup(P))
