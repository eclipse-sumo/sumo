#ifndef lint
static const char SCCSID[]="@(#)PJ_putp4p.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	C_x, C_y;
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(putp4p, "Putnins P4'") "\n\tPCyl., Sph.";
PROJ_HEAD(weren, "Werenskiold I") "\n\tPCyl., Sph.";
FORWARD(s_forward); /* spheroid */
	lp.phi = aasin(0.883883476 * sin(lp.phi));
	xy.x = P->C_x * lp.lam * cos(lp.phi);
	xy.x /= cos(lp.phi *= 0.333333333333333);
	xy.y = P->C_y * sin(lp.phi);
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.phi = aasin(xy.y / P->C_y);
	lp.lam = xy.x * cos(lp.phi) / P->C_x;
	lp.phi *= 3.;
	lp.lam /= cos(lp.phi);
	lp.phi = aasin(1.13137085 * sin(lp.phi));
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
	static PJ *
setup(PJ *P) {
	P->es = 0.; P->inv = s_inverse; P->fwd = s_forward;
	return P;
}
ENTRY0(putp4p) P->C_x = 0.874038744; P->C_y = 3.883251825; ENDENTRY(setup(P))
ENTRY0(weren) P->C_x = 1.; P->C_y = 4.442882938; ENDENTRY(setup(P))
