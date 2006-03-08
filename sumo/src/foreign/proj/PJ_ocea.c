#ifndef lint
static const char SCCSID[]="@(#)PJ_ocea.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	rok; \
	double	rtk; \
	double	sinphi; \
	double	cosphi; \
	double	singam; \
	double	cosgam;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(ocea, "Oblique Cylindrical Equal Area") "\n\tCyl, Sph"
	"lonc= alpha= or\n\tlat_1= lat_2= lon_1= lon_2=";
FORWARD(s_forward); /* spheroid */
	double t;

	xy.y = sin(lp.lam);
/*
	xy.x = atan2((tan(lp.phi) * P->cosphi + P->sinphi * xy.y) , cos(lp.lam));
*/
	t = cos(lp.lam);
	xy.x = atan((tan(lp.phi) * P->cosphi + P->sinphi * xy.y) / t);
	if (t < 0.)
		xy.x += PI;
	xy.x *= P->rtk;
	xy.y = P->rok * (P->sinphi * sin(lp.phi) - P->cosphi * cos(lp.phi) * xy.y);
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double t, s;

	xy.y /= P->rok;
	xy.x /= P->rtk;
	t = sqrt(1. - xy.y * xy.y);
	lp.phi = asin(xy.y * P->sinphi + t * P->cosphi * (s = sin(xy.x)));
	lp.lam = atan2(t * P->sinphi * s - xy.y * P->cosphi,
		t * cos(xy.x));
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(ocea)
	double phi_0=0.0, phi_1, phi_2, lam_1, lam_2, lonz, alpha;

	P->rok = P->a / P->k0;
	P->rtk = P->a * P->k0;
	if ( pj_param(P->params, "talpha").i) {
		alpha	= pj_param(P->params, "ralpha").f;
		lonz = pj_param(P->params, "rlonc").f;
		P->singam = atan(-cos(alpha)/(-sin(phi_0) * sin(alpha))) + lonz;
		P->sinphi = asin(cos(phi_0) * sin(alpha));
	} else {
		phi_1 = pj_param(P->params, "rlat_1").f;
		phi_2 = pj_param(P->params, "rlat_2").f;
		lam_1 = pj_param(P->params, "rlon_1").f;
		lam_2 = pj_param(P->params, "rlon_2").f;
		P->singam = atan2(cos(phi_1) * sin(phi_2) * cos(lam_1) -
			sin(phi_1) * cos(phi_2) * cos(lam_2),
			sin(phi_1) * cos(phi_2) * sin(lam_2) -
			cos(phi_1) * sin(phi_2) * sin(lam_1) );
		P->sinphi = atan(-cos(P->singam - lam_1) / tan(phi_1));
	}
	P->lam0 = P->singam + HALFPI;
	P->cosphi = cos(P->sinphi);
	P->sinphi = sin(P->sinphi);
	P->cosgam = cos(P->singam);
	P->singam = sin(P->singam);
	P->inv = s_inverse;
	P->fwd = s_forward;
	P->es = 0.;
ENDENTRY(P)
