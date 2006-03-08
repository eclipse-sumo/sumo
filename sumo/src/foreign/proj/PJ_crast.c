#ifndef lint
static const char SCCSID[]="@(#)PJ_crast.c	4.1 94/02/15     GIE     REL";
#endif
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(crast, "Craster Parabolic (Putnins P4)")
"\n\tPCyl., Sph.";
#define XM	0.97720502380583984317
#define RXM	1.02332670794648848847
#define YM	3.06998012383946546542
#define RYM	0.32573500793527994772
#define THIRD	0.333333333333333333
FORWARD(s_forward); /* spheroid */
	lp.phi *= THIRD;
	xy.x = XM * lp.lam * (2. * cos(lp.phi + lp.phi) - 1.);
	xy.y = YM * sin(lp.phi);
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.phi = 3. * asin(xy.y * RYM);
	lp.lam = xy.x * RXM / (2. * cos((lp.phi + lp.phi) * THIRD) - 1);
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(crast) P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
