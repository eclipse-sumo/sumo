#ifndef lint
static const char SCCSID[]="@(#)PJ_mpoly.c	4.1	94/05/22	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	rho_0;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(mpoly, "Modified Polyconic")
	"\n\tPolyconic, Sph\n\tlat_1= and lat_2= lotsa";
FORWARD(s_forward); /* spheroid */
	return (xy);
}
INVERSE(s_inverse); /* ellipsoid & spheroid */
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(mpoly)
	P->fwd = s_forward;
	P->inv = s_inverse;
	P->es = 0;
ENDENTRY(P)
