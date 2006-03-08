#ifndef lint
static const char SCCSID[]="@(#)PJ_denoy.c	4.1 94/02/15     GIE     REL";
#endif
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(denoy, "Denoyer Semi-Elliptical") "\n\tPCyl., no inv., Sph.";
#define C0	0.95
#define C1	-.08333333333333333333
#define C3	.00166666666666666666
#define D1	0.9
#define D5	0.03
FORWARD(s_forward); /* spheroid */
	xy.y = lp.phi;
	xy.x = lp.lam;
	lp.lam = fabs(lp.lam);
	xy.x *= cos((C0 + lp.lam * (C1 + lp.lam * lp.lam * C3)) *
		(lp.phi * (D1 + D5 * lp.phi * lp.phi * lp.phi * lp.phi)));
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(denoy) P->es = 0.; P->fwd = s_forward; ENDENTRY(P)
