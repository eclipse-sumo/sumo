#ifndef lint
static const char SCCSID[]="@(#)PJ_chamb.c	4.1	94/02/15	GIE	REL";
#endif
typedef struct { double r, Az; } VECT;
#define PROJ_PARMS__ \
	struct { /* control point data */ \
		double phi, lam; \
		double cosphi, sinphi; \
		VECT v; \
		XY	p; \
		double Az; \
	} c[3]; \
	XY p; \
	double beta_0, beta_1, beta_2;
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(chamb, "Chamberlin Trimetric") "\n\tMisc Sph, no inv."
"\n\tlat_1= lon_1= lat_2= lon_2= lat_3= lon_3=";
#include	<stdio.h>
#define THIRD 0.333333333333333333
#define TOL 1e-9
	static VECT /* distance and azimuth from point 1 to point 2 */
vect(double dphi, double c1, double s1, double c2, double s2, double dlam) {
	VECT v;
	double cdl, dp, dl;

	cdl = cos(dlam);
	if (fabs(dphi) > 1. || fabs(dlam) > 1.)
		v.r = aacos(s1 * s2 + c1 * c2 * cdl);
	else { /* more accurate for smaller distances */
		dp = sin(.5 * dphi);
		dl = sin(.5 * dlam);
		v.r = 2. * aasin(sqrt(dp * dp + c1 * c2 * dl * dl));
	}
	if (fabs(v.r) > TOL)
		v.Az = atan2(c2 * sin(dlam), c1 * s2 - s1 * c2 * cdl);
	else
		v.r = v.Az = 0.;
	return v;
}
	static double /* law of cosines */
lc(double b,double c,double a) {
	return aacos(.5 * (b * b + c * c - a * a) / (b * c));
}
FORWARD(s_forward); /* spheroid */
	double sinphi, cosphi, a;
	VECT v[3];
	int i, j;

	sinphi = sin(lp.phi);
	cosphi = cos(lp.phi);
	for (i = 0; i < 3; ++i) { /* dist/azimiths from control */
		v[i] = vect(lp.phi - P->c[i].phi, P->c[i].cosphi, P->c[i].sinphi,
			cosphi, sinphi, lp.lam - P->c[i].lam);
		if ( ! v[i].r)
			break;
		v[i].Az = adjlon(v[i].Az - P->c[i].v.Az);
	}
	if (i < 3) /* current point at control point */
		xy = P->c[i].p;
	else { /* point mean of intersepts */
		xy = P->p;
		for (i = 0; i < 3; ++i) {
			j = i == 2 ? 0 : i + 1;
			a = lc(P->c[i].v.r, v[i].r, v[j].r);
			if (v[i].Az < 0.)
				a = -a;
			if (! i) { /* coord comp unique to each arc */
				xy.x += v[i].r * cos(a);
				xy.y -= v[i].r * sin(a);
			} else if (i == 1) {
				a = P->beta_1 - a;
				xy.x -= v[i].r * cos(a);
				xy.y -= v[i].r * sin(a);
			} else {
				a = P->beta_2 - a;
				xy.x += v[i].r * cos(a);
				xy.y += v[i].r * sin(a);
			}
		}
		xy.x *= THIRD; /* mean of arc intercepts */
		xy.y *= THIRD;
	}
	return xy;
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(chamb)
	int i, j;
	char line[10];

	for (i = 0; i < 3; ++i) { /* get control point locations */
		(void)sprintf(line, "rlat_%d", i+1);
		P->c[i].phi = pj_param(P->params, line).f;
		(void)sprintf(line, "rlon_%d", i+1);
		P->c[i].lam = pj_param(P->params, line).f;
		P->c[i].lam = adjlon(P->c[i].lam - P->lam0);
		P->c[i].cosphi = cos(P->c[i].phi);
		P->c[i].sinphi = sin(P->c[i].phi);
	}
	for (i = 0; i < 3; ++i) { /* inter ctl pt. distances and azimuths */
		j = i == 2 ? 0 : i + 1;
		P->c[i].v = vect(P->c[j].phi - P->c[i].phi, P->c[i].cosphi, P->c[i].sinphi,
			P->c[j].cosphi, P->c[j].sinphi, P->c[j].lam - P->c[i].lam);
		if (! P->c[i].v.r) E_ERROR(-25);
		/* co-linearity problem ignored for now */
	}
	P->beta_0 = lc(P->c[0].v.r, P->c[2].v.r, P->c[1].v.r);
	P->beta_1 = lc(P->c[0].v.r, P->c[1].v.r, P->c[2].v.r);
	P->beta_2 = PI - P->beta_0;
	P->p.y = 2. * (P->c[0].p.y = P->c[1].p.y = P->c[2].v.r * sin(P->beta_0));
	P->c[2].p.y = 0.;
	P->c[0].p.x = - (P->c[1].p.x = 0.5 * P->c[0].v.r);
	P->p.x = P->c[2].p.x = P->c[0].p.x + P->c[2].v.r * cos(P->beta_0);
	P->es = 0.; P->fwd = s_forward;
ENDENTRY(P)
