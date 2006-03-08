#ifndef lint
static const char SCCSID[]="@(#)PJ_eck5.c	4.1 94/02/15     GIE     REL";
#endif
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(eck5, "Eckert V") "\n\tPCyl, Sph.";
#define XF	0.44101277172455148219
#define RXF	2.26750802723822639137
#define YF	0.88202554344910296438
#define RYF	1.13375401361911319568
FORWARD(s_forward); /* spheroid */
	xy.x = XF * (1. + cos(lp.phi)) * lp.lam;
	xy.y = YF * lp.phi;
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.lam = RXF * xy.x / (1. + cos( lp.phi = RYF * xy.y));
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(eck5); P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
