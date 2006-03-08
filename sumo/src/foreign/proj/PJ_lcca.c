static const char RCS_ID[] = "$Id$";
/* PROJ.4 Cartographic Projection System -- Revision Log:
**$Log$
**Revision 1.1  2006/03/08 13:02:28  dkrajzew
**some further work on converting geo-coordinates
**
**Revision 1.1  2003/03/04 02:59:41  warmerda
**New
**
*/
#define MAX_ITER 10
#define DEL_TOL 1e-12
#define PROJ_PARMS__ \
	double	*en; \
	double	r0, l, M0; \
	double	C;
#define PJ_LIB__
#include	<projects.h>

PROJ_HEAD(lcca, "Lambert Conformal Conic Alternative")
	"\n\tConic, Sph&Ell\n\tlat_0=";

	static double /* func to compute dr */
fS(double S, double C) {
		return(S * ( 1. + S * S * C));
}
	static double /* deriv of fs */
fSp(double S, double C) {
	return(1. + 3.* S * S * C);
}
FORWARD(e_forward); /* ellipsoid */
	double S, S3, r, dr;

	S = pj_mlfn(lp.phi, sin(lp.phi), cos(lp.phi), P->en) - P->M0;
	dr = fS(S, P->C);
	r = P->r0 - dr;
	xy.x = P->k0 * (r * sin( lp.lam *= P->l ) );
	xy.y = P->k0 * (P->r0 - r * cos(lp.lam) );
	return (xy);
}
INVERSE(e_inverse); /* ellipsoid & spheroid */
	double theta, dr, S, dif;
	int i;

	xy.x /= P->k0;
	xy.y /= P->k0;
	theta = atan2(xy.x , P->r0 - xy.y);
	dr = xy.y - xy.x * tan(0.5 * theta);
	lp.lam = theta / P->l;
	S = dr;
	for (i = MAX_ITER; i ; --i) {
		S -= (dif = (fS(S, P->C) - dr) / fSp(S, P->C));
		if (fabs(dif) < DEL_TOL) break;
	}
	if (!i) I_ERROR
	lp.phi = pj_inv_mlfn(S + P->M0, P->es, P->en);
	return (lp);
}
FREEUP; if (P) { if (P->en) pj_dalloc(P->en); pj_dalloc(P); } }
ENTRY0(lcca)
	double s2p0, N0, R0, tan0, tan20;

	if (!(P->en = pj_enfn(P->es))) E_ERROR_0;
	if (!pj_param(P->params, "tlat_0").i) E_ERROR(50);
	if (P->phi0 == 0.) E_ERROR(51);
	P->l = sin(P->phi0);
	P->M0 = pj_mlfn(P->phi0, P->l, cos(P->phi0), P->en);
	s2p0 = P->l * P->l;
	R0 = 1. / (1. - P->es * s2p0);
	N0 = sqrt(R0);
	R0 *= P->one_es * N0;
	tan0 = tan(P->phi0);
	tan20 = tan0 * tan0;
	P->r0 = N0 / tan0;
	P->C = 1. / (6. * R0 * N0);
	P->inv = e_inverse;
	P->fwd = e_forward;
ENDENTRY(P)

