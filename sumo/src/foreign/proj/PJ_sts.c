#ifndef lint
static const char SCCSID[]="@(#)PJ_sts.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double C_x, C_y, C_p; \
	int tan_mode;
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(kav5, "Kavraisky V") "\n\tPCyl., Sph.";
PROJ_HEAD(qua_aut, "Quartic Authalic") "\n\tPCyl., Sph.";
PROJ_HEAD(mbt_s, "McBryde-Thomas Flat-Polar Sine (No. 1)") "\n\tPCyl., Sph.";
PROJ_HEAD(fouc, "Foucaut") "\n\tPCyl., Sph.";
FORWARD(s_forward); /* spheroid */
	double c;

	xy.x = P->C_x * lp.lam * cos(lp.phi);
	xy.y = P->C_y;
	lp.phi *= P->C_p;
	c = cos(lp.phi);
	if (P->tan_mode) {
		xy.x *= c * c;
		xy.y *= tan(lp.phi);
	} else {
		xy.x /= c;
		xy.y *= sin(lp.phi);
	}
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double c;

	xy.y /= P->C_y;
	c = cos(lp.phi = P->tan_mode ? atan(xy.y) : aasin(xy.y));
	lp.phi /= P->C_p;
	lp.lam = xy.x / (P->C_x * cos(lp.phi /= P->C_p));
	if (P->tan_mode)
		lp.lam /= c * c;
	else
		lp.lam *= c;
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
	static PJ *
setup(PJ *P, double p, double q, int mode) {
	P->es = 0.;
	P->inv = s_inverse;
	P->fwd = s_forward;
	P->C_x = q / p;
	P->C_y = p;
	P->C_p = 1/ q;
	P->tan_mode = mode;
	return P;
}
ENTRY0(kav5) ENDENTRY(setup(P, 1.50488, 1.35439, 0))
ENTRY0(qua_aut) ENDENTRY(setup(P, 2., 2., 0))
ENTRY0(mbt_s) ENDENTRY(setup(P, 1.48875, 1.36509, 0))
ENTRY0(fouc) ENDENTRY(setup(P, 2., 2., 1))
