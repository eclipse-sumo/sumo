#ifndef lint
static const char SCCSID[]="@(#)PJ_urm5.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double m, rmn, q3, n;
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(urm5, "Urmaev V") "\n\tPCyl., Sph.\n\tn= q= alphi=";
FORWARD(s_forward); /* spheroid */
	double t;

	t = lp.phi = aasin(P->n * sin(lp.phi));
	xy.x = P->m * lp.lam * cos(lp.phi);
	t *= t;
	xy.y = lp.phi * (1. + t * P->q3) * P->rmn;
	return xy;
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(urm5)
	double alpha, t;

	P->n = pj_param(P->params, "dn").f;
	P->q3 = pj_param(P->params, "dq").f / 3.;
	alpha = pj_param(P->params, "ralpha").f;
	t = P->n * sin(alpha);
	P->m = cos(alpha) / sqrt(1. - t * t);
	P->rmn = 1. / (P->m * P->n);
	P->es = 0.;
	P->inv = 0;
	P->fwd = s_forward;
ENDENTRY(P)
