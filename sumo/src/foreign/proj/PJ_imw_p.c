#ifndef lint
static const char SCCSID[]="@(#)PJ_imw_p.c	4.1	94/05/22	GIE	REL";
#endif
#define PROJ_PARMS__ \
	double	P, Pp, Q, Qp, R_1, R_2, sphi_1, sphi_2, C2; \
	double	phi_1, phi_2, lam_1; \
	double	*en; \
	int	mode; /* = 0, phi_1 and phi_2 != 0, = 1, phi_1 = 0, = -1 phi_2 = 0 */
#define PJ_LIB__
#include	<projects.h>
PROJ_HEAD(imw_p, "International Map of the World Polyconic")
	"\n\tMod. Polyconic, Ell\n\tlat_1= and lat_2= [lon_1=]";
#define TOL 1e-10
#define EPS 1e-10
	static int
phi12(PJ *P, double *del, double *sig) {
	int err = 0;

	if (!pj_param(P->params, "tlat_1").i ||
		!pj_param(P->params, "tlat_2").i) {
		err = -41;
	} else {
		P->phi_1 = pj_param(P->params, "rlat_1").f;
		P->phi_2 = pj_param(P->params, "rlat_2").f;
		*del = 0.5 * (P->phi_2 - P->phi_1);
		*sig = 0.5 * (P->phi_2 + P->phi_1);
		err = (fabs(*del) < EPS || fabs(*sig) < EPS) ? -42 : 0;
	}
	return err;
}
	static XY
loc_for(LP lp, PJ *P, double *yc) {
	XY xy;

	if (! lp.phi) {
		xy.x = lp.lam;
		xy.y = 0.;
	} else {
		double xa, ya, xb, yb, xc, yc, D, B, m, sp, t, R, C;

		sp = sin(lp.phi);
		m = pj_mlfn(lp.phi, sp, cos(lp.phi), P->en);
		xa = P->Pp + P->Qp * m;
		ya = P->P + P->Q * m;
		R = 1. / (tan(lp.phi) * sqrt(1. - P->es * sp * sp));
		C = sqrt(R * R - xa * xa);
		if (lp.phi < 0.) C = - C;
		C += ya - R;
		if (P->mode < 0) {
			xb = lp.lam;
			yb = P->C2;
		} else {
			t = lp.lam * P->sphi_2;
			xb = P->R_2 * sin(t);
			yb = P->C2 + P->R_2 * (1. - cos(t));
		}
		if (P->mode > 0) {
			xc = lp.lam;
			yc = 0.;
		} else {
			t = lp.lam * P->sphi_1;
			xc = P->R_1 * sin(t);
			yc = P->R_1 * (1. - cos(t));
		}
		D = (xb - xc)/(yb - yc);
		B = xc + D * (C + R - yc);
		xy.x = D * sqrt(R * R * (1 + D * D) - B * B);
		if (lp.phi > 0)
			xy.x = - xy.x;
		xy.x = (B + xy.x) / (1. + D * D);
		xy.y = sqrt(R * R - xy.x * xy.x);
		if (lp.phi > 0)
			xy.y = - xy.y;
		xy.y += C + R;
	}
	return (xy);
}
FORWARD(e_forward); /* ellipsoid */
	double yc;
	xy = loc_for(lp, P, &yc);
	return (xy);
}
INVERSE(e_inverse); /* ellipsoid */
	XY t;
	double yc;

	lp.phi = P->phi_2;
	lp.lam = xy.x / cos(lp.phi);
	do {
		t = loc_for(lp, P, &yc);
		lp.phi = ((lp.phi - P->phi_1) * (xy.y - yc) / (t.y - yc)) + P->phi_1;
		lp.lam = lp.lam * xy.x / t.x;
	} while (fabs(t.x - xy.x) > TOL || fabs(t.y - xy.y) > TOL);
	return (lp);
}
	static void
xy(PJ *P, double phi, double *x, double *y, double *sp, double *R) {
	double F;

	*sp = sin(phi);
	*R = 1./(tan(phi) * sqrt(1. - P->es * *sp * *sp ));
	F = P->lam_1 * *sp;
	*y = *R * (1 - cos(F));
	*x = *R * sin(F);
}
FREEUP; if (P) { if (P->en) pj_dalloc(P->en); pj_dalloc(P); } }
ENTRY1(imw_p, en)
	double del, sig, s, t, x1, x2, T2, y1, m1, m2, y2;
	int i;

	if (!(P->en = pj_enfn(P->es))) E_ERROR_0;
	if( (i = phi12(P, &del, &sig)) != 0)
		E_ERROR(i);
	if (P->phi_2 < P->phi_1) { /* make sure P->phi_1 most southerly */
		del = P->phi_1;
		P->phi_1 = P->phi_2;
		P->phi_2 = del;
	}
	if (pj_param(P->params, "tlon_1").i)
		P->lam_1 = pj_param(P->params, "rlon_1").f;
	else { /* use predefined based upon latitude */
		sig = fabs(sig * RAD_TO_DEG);
		if (sig <= 60)		sig = 2.;
		else if (sig <= 76) sig = 4.;
		else				sig = 8.;
		P->lam_1 = sig * DEG_TO_RAD;
	}
	P->mode = 0;
	if (P->phi_1) xy(P, P->phi_1, &x1, &y1, &P->sphi_1, &P->R_1);
	else {
		P->mode = 1;
		y1 = 0.;
		x1 = P->lam_1;
	}
	if (P->phi_2) xy(P, P->phi_2, &x2, &T2, &P->sphi_2, &P->R_2);
	else {
		P->mode = -1;
		T2 = 0.;
		x2 = P->lam_1;
	}
	m1 = pj_mlfn(P->phi_1, P->sphi_1, cos(P->phi_1), P->en);
	m2 = pj_mlfn(P->phi_2, P->sphi_2, cos(P->phi_2), P->en);
	t = m2 - m1;
	s = x2 - x1;
	y2 = sqrt(t * t - s * s) + y1;
	P->C2 = y2 - T2;
	t = 1. / t;
	P->P = (m2 * y1 - m1 * y2) * t;
	P->Q = (y2 - y1) * t;
	P->Pp = (m2 * x1 - m1 * x2) * t;
	P->Qp = (x2 - x1) * t;
	P->fwd = e_forward;
	P->inv = e_inverse;
ENDENTRY(P)
