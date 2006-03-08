#ifndef lint
static const char SCCSID[]="@(#)PJ_wag3.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	C_x;
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(wag3, "Wagner III") "\n\tPCyl., Sph.";
#define TWOTHIRD 0.6666666666666666666667
FORWARD(s_forward); /* spheroid */
	xy.x = P->C_x * lp.lam * cos(TWOTHIRD * lp.phi);
	xy.y = lp.phi;
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.phi = xy.y;
	lp.lam = xy.x / (P->C_x * cos(TWOTHIRD * lp.phi));
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(wag3)
	double ts;

	ts = pj_param(P->params, "rlat_ts").f;
	P->C_x = cos(ts) / cos(2.*ts/3.);
	P->es = 0.; P->inv = s_inverse; P->fwd = s_forward;
ENDENTRY(P)
