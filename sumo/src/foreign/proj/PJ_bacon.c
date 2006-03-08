#ifndef lint
static const char SCCSID[]="@(#)PJ_bacon.c	4.1	94/02/15	GIE	REL";
#endif
# define HLFPI2	2.46740110027233965467
# define EPS	1e-10
#define PROJ_PARMS__ \
	int bacn; \
	int ortl;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(apian, "Apian Globular I") "\n\tMisc Sph, no inv.";
PROJ_HEAD(ortel, "Ortelius Oval") "\n\tMisc Sph, no inv.";
PROJ_HEAD(bacon, "Bacon Globular") "\n\tMisc Sph, no inv.";
FORWARD(s_forward); /* spheroid */
	double ax, f;

	xy.y = P->bacn ? HALFPI * sin(lp.phi) : lp.phi;
	if ((ax = fabs(lp.lam)) >= EPS) {
		if (P->ortl && ax >= HALFPI)
			xy.x = sqrt(HLFPI2 - lp.phi * lp.phi + EPS) + ax - HALFPI;
		else {
			f = 0.5 * (HLFPI2 / ax + ax);
			xy.x = ax - f + sqrt(f * f - xy.y * xy.y);
		}
		if (lp.lam < 0.) xy.x = - xy.x;
	} else
		xy.x = 0.;
	return (xy);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(bacon)
	P->bacn = 1;
	P->ortl = 0;
	P->es = 0.; P->fwd = s_forward;
ENDENTRY(P)
ENTRY0(apian)
	P->bacn = P->ortl = 0;
	P->es = 0.; P->fwd = s_forward;
ENDENTRY(P)
ENTRY0(ortel)
	P->bacn = 0;
	P->ortl = 1;
	P->es = 0.; P->fwd = s_forward;
ENDENTRY(P)
