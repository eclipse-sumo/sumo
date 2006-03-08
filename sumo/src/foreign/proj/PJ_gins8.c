#ifndef lint
static const char SCCSID[]="@(#)PJ_gins8.c	4.1	94/02/15	GIE	REL";
#endif
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(gins8, "Ginsburg VIII (TsNIIGAiK)") "\n\tPCyl, Sph., no inv.";
#define	Cl 0.000952426
#define	Cp 0.162388
#define	C12 0.08333333333333333
FORWARD(s_forward); /* spheroid */
	double t = lp.phi * lp.phi;

	xy.y = lp.phi * (1. + t * C12);
	xy.x = lp.lam * (1. - Cp * t);
	t = lp.lam * lp.lam;
	xy.x *= (0.87 - Cl * t * t);
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(gins8) P->es = 0.; P->inv = 0; P->fwd = s_forward; ENDENTRY(P)
