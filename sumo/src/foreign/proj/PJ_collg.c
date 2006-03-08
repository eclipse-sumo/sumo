#ifndef lint
static const char SCCSID[]="@(#)PJ_collg.c	4.1 94/02/15     GIE     REL";
#endif
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(collg, "Collignon") "\n\tPCyl, Sph.";
#define FXC	1.12837916709551257390
#define FYC	1.77245385090551602729
#define ONEEPS	1.0000001
FORWARD(s_forward); /* spheroid */
	if ((xy.y = 1. - sin(lp.phi)) <= 0.)
		xy.y = 0.;
	else
		xy.y = sqrt(xy.y);
	xy.x = FXC * lp.lam * xy.y;
	xy.y = FYC * (1. - xy.y);
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.phi = xy.y / FYC - 1.;
	if (fabs(lp.phi = 1. - lp.phi * lp.phi) < 1.)
		lp.phi = asin(lp.phi);
	else if (fabs(lp.phi) > ONEEPS) I_ERROR
	else	lp.phi = lp.phi < 0. ? -HALFPI : HALFPI;
	if ((lp.lam = 1. - sin(lp.phi)) <= 0.)
		lp.lam = 0.;
	else
		lp.lam = xy.x / (FXC * sqrt(lp.lam));
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(collg) P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
