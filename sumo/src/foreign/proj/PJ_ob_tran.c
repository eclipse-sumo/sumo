#ifndef lint
static const char SCCSID[]="@(#)PJ_ob_tran.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	struct PJconsts *link; \
	double	lamp; \
	double	cphip, sphip;
#define PJ_LIB__
#include <projects.h>
#include <string.h>
PROJ_HEAD(ob_tran, "General Oblique Transformation") "\n\tMisc Sph"
"\n\to_proj= plus parameters for projection"
"\n\to_lat_p= o_lon_p= (new pole) or"
"\n\to_alpha= o_lon_c= o_lat_c= or"
"\n\to_lon_1= o_lat_1= o_lon_2= o_lat_2=";
#define TOL 1e-10
FORWARD(o_forward); /* spheroid */
	double coslam, sinphi, cosphi;

        (void) xy;

	coslam = cos(lp.lam);
	sinphi = sin(lp.phi);
	cosphi = cos(lp.phi);
	lp.lam = adjlon(aatan2(cosphi * sin(lp.lam), P->sphip * cosphi * coslam +
		P->cphip * sinphi) + P->lamp);
	lp.phi = aasin(P->sphip * sinphi - P->cphip * cosphi * coslam);
	return (P->link->fwd(lp, P->link));
}
FORWARD(t_forward); /* spheroid */
	double cosphi, coslam;

        (void) xy;

	cosphi = cos(lp.phi);
	coslam = cos(lp.lam);
	lp.lam = adjlon(aatan2(cosphi * sin(lp.lam), sin(lp.phi)) + P->lamp);
	lp.phi = aasin(- cosphi * coslam);
	return (P->link->fwd(lp, P->link));
}
INVERSE(o_inverse); /* spheroid */
	double coslam, sinphi, cosphi;

	lp = P->link->inv(xy, P->link);
	if (lp.lam != HUGE_VAL) {
		coslam = cos(lp.lam -= P->lamp);
		sinphi = sin(lp.phi);
		cosphi = cos(lp.phi);
		lp.phi = aasin(P->sphip * sinphi + P->cphip * cosphi * coslam);
		lp.lam = aatan2(cosphi * sin(lp.lam), P->sphip * cosphi * coslam -
			P->cphip * sinphi);
	}
	return (lp);
}
INVERSE(t_inverse); /* spheroid */
	double cosphi, t;

	lp = P->link->inv(xy, P->link);
	if (lp.lam != HUGE_VAL) {
		cosphi = cos(lp.phi);
		t = lp.lam - P->lamp;
		lp.lam = aatan2(cosphi * sin(t), - sin(lp.phi));
		lp.phi = aasin(cosphi * cos(t));
	}
	return (lp);
}
FREEUP;
	if (P) {
		if (P->link)
			(*(P->link->pfree))(P->link);
		pj_dalloc(P);
	}
}
ENTRY1(ob_tran, link)
	int i;
	double phip;
	char *name, *s;

	/* get name of projection to be translated */
	if (!(name = pj_param(P->params, "so_proj").s)) E_ERROR(-26);
	for (i = 0; (s = pj_list[i].id) && strcmp(name, s) ; ++i) ;
	if (!s || !(P->link = (*pj_list[i].proj)(0))) E_ERROR(-37);
	/* copy existing header into new */
	P->es = 0.; /* force to spherical */
	P->link->params = P->params;
	P->link->over = P->over;
	P->link->geoc = P->geoc;
	P->link->a = P->a;
	P->link->es = P->es;
	P->link->ra = P->ra;
	P->link->lam0 = P->lam0;
	P->link->phi0 = P->phi0;
	P->link->x0 = P->x0;
	P->link->y0 = P->y0;
	P->link->k0 = P->k0;
	/* force spherical earth */
	P->link->one_es = P->link->rone_es = 1.;
	P->link->es = P->link->e = 0.;
	if (!(P->link = pj_list[i].proj(P->link))) {
		freeup(P);
		return 0;
	}
	if (pj_param(P->params, "to_alpha").i) {
		double lamc, phic, alpha;

		lamc	= pj_param(P->params, "ro_lon_c").f;
		phic	= pj_param(P->params, "ro_lat_c").f;
		alpha	= pj_param(P->params, "ro_alpha").f;
/*
		if (fabs(phic) <= TOL ||
			fabs(fabs(phic) - HALFPI) <= TOL ||
			fabs(fabs(alpha) - HALFPI) <= TOL)
*/
		if (fabs(fabs(phic) - HALFPI) <= TOL)
			E_ERROR(-32);
		P->lamp = lamc + aatan2(-cos(alpha), -sin(alpha) * sin(phic));
		phip = aasin(cos(phic) * sin(alpha));
	} else if (pj_param(P->params, "to_lat_p").i) { /* specified new pole */
		P->lamp = pj_param(P->params, "ro_lon_p").f;
		phip = pj_param(P->params, "ro_lat_p").f;
	} else { /* specified new "equator" points */
		double lam1, lam2, phi1, phi2, con;

		lam1 = pj_param(P->params, "ro_lon_1").f;
		phi1 = pj_param(P->params, "ro_lat_1").f;
		lam2 = pj_param(P->params, "ro_lon_2").f;
		phi2 = pj_param(P->params, "ro_lat_2").f;
		if (fabs(phi1 - phi2) <= TOL ||
			(con = fabs(phi1)) <= TOL ||
			fabs(con - HALFPI) <= TOL ||
			fabs(fabs(phi2) - HALFPI) <= TOL) E_ERROR(-33);
		P->lamp = atan2(cos(phi1) * sin(phi2) * cos(lam1) -
			sin(phi1) * cos(phi2) * cos(lam2),
			sin(phi1) * cos(phi2) * sin(lam2) -
			cos(phi1) * sin(phi2) * sin(lam1));
		phip = atan(-cos(P->lamp - lam1) / tan(phi1));
	}
	if (fabs(phip) > TOL) { /* oblique */
		P->cphip = cos(phip);
		P->sphip = sin(phip);
		P->fwd = o_forward;
		P->inv = P->link->inv ? o_inverse : 0;
	} else { /* transverse */
		P->fwd = t_forward;
		P->inv = P->link->inv ? t_inverse : 0;
	}
ENDENTRY(P)
