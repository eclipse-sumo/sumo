#ifndef lint
static const char SCCSID[]="@(#)PJ_eck1.c	4.1 94/02/15     GIE     REL";
#endif
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(eck1, "Eckert I") "\n\tPCyl., Sph.";
#define FC	.92131773192356127802
#define RP	.31830988618379067154
FORWARD(s_forward); /* spheroid */
	xy.x = FC * lp.lam * (1. - RP * fabs(lp.phi));
	xy.y = FC * lp.phi;
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.phi = xy.y / FC;
	lp.lam = xy.x / (FC * (1. - RP * fabs(lp.phi)));
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(eck1)
	P->es = 0.; P->inv = s_inverse; P->fwd = s_forward;
ENDENTRY(P)
