#ifndef lint
static const char SCCSID[]="@(#)PJ_gall.c	4.1	94/02/15	GIE	REL";
#endif
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(gall, "Gall (Gall Stereographic)") "\n\tCyl, Sph";
#define YF	1.70710678118654752440
#define XF	0.70710678118654752440
#define RYF	0.58578643762690495119
#define RXF	1.41421356237309504880
FORWARD(s_forward); /* spheroid */
	xy.x = XF * lp.lam;
	xy.y = YF * tan(.5 * lp.phi);
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.lam = RXF * xy.x;
	lp.phi = 2. * atan(xy.y * RYF);
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(gall) P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
