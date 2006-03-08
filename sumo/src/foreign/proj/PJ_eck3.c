#ifndef lint
static const char SCCSID[]="@(#)PJ_eck3.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double C_x, C_y, A, B;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(eck3, "Eckert III") "\n\tPCyl, Sph.";
PROJ_HEAD(putp1, "Putnins P1") "\n\tPCyl, Sph.";
PROJ_HEAD(wag6, "Wagner VI") "\n\tPCyl, Sph.";
PROJ_HEAD(kav7, "Kavraisky VII") "\n\tPCyl, Sph.";
FORWARD(s_forward); /* spheroid */
	xy.y = P->C_y * lp.phi;
	xy.x = P->C_x * lp.lam * (P->A + asqrt(1. - P->B * lp.phi * lp.phi));
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.phi = xy.y / P->C_y;
	lp.lam = xy.x / (P->C_x * (P->A + asqrt(1. - P->B * lp.phi * lp.phi)));
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
ENTRY0(eck3)
	P->C_x = .42223820031577120149;
	P->C_y = .84447640063154240298;
	P->A = 1.;
	P->B = 0.4052847345693510857755;
ENDENTRY(setup(P))
ENTRY0(kav7)
	P->C_x = 0.2632401569273184856851;
	P->C_x = 0.8660254037844;
	P->C_y = 1.;
	P->A = 0.;
	P->B = 0.30396355092701331433;
ENDENTRY(setup(P))
ENTRY0(wag6);
	P->C_x = P->C_y = 0.94745;
	P->A = 0.;
	P->B = 0.30396355092701331433;
ENDENTRY(setup(P))
ENTRY0(putp1);
	P->C_x = 1.89490;
	P->C_y = 0.94745;
	P->A = -0.5;
	P->B = 0.30396355092701331433;
ENDENTRY(setup(P))
