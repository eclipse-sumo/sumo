#ifndef lint
static const char SCCSID[]="@(#)PJ_bipc.c	4.1	94/02/15	GIE	REL";
#endif
#define PROJ_PARMS__ \
	int	noskew;
#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(bipc, "Bipolar conic of western hemisphere")
	"\n\tConic Sph.";
# define EPS	1e-10
# define EPS10	1e-10
# define ONEEPS 1.000000001
# define NITER	10
# define lamB	-.34894976726250681539
# define n	.63055844881274687180
# define F	1.89724742567461030582
# define Azab	.81650043674686363166
# define Azba	1.82261843856185925133
# define T	1.27246578267089012270
# define rhoc	1.20709121521568721927
# define cAzc	.69691523038678375519
# define sAzc	.71715351331143607555
# define C45	.70710678118654752469
# define S45	.70710678118654752410
# define C20	.93969262078590838411
# define S20	-.34202014332566873287
# define R110	1.91986217719376253360
# define R104	1.81514242207410275904
FORWARD(s_forward); /* spheroid */
	double cphi, sphi, tphi, t, al, Az, z, Av, cdlam, sdlam, r;
	int tag;

	cphi = cos(lp.phi);
	sphi = sin(lp.phi);
	cdlam = cos(sdlam = lamB - lp.lam);
	sdlam = sin(sdlam);
	if (fabs(fabs(lp.phi) - HALFPI) < EPS10) {
		Az = lp.phi < 0. ? PI : 0.;
		tphi = HUGE_VAL;
	} else {
		tphi = sphi / cphi;
		Az = atan2(sdlam , C45 * (tphi - cdlam));
	}
	if( (tag = (Az > Azba)) ) {
		cdlam = cos(sdlam = lp.lam + R110);
		sdlam = sin(sdlam);
		z = S20 * sphi + C20 * cphi * cdlam;
		if (fabs(z) > 1.) {
			if (fabs(z) > ONEEPS) F_ERROR
			else z = z < 0. ? -1. : 1.;
		} else
			z = acos(z);
		if (tphi != HUGE_VAL)
			Az = atan2(sdlam, (C20 * tphi - S20 * cdlam));
		Av = Azab;
		xy.y = rhoc;
	} else {
		z = S45 * (sphi + cphi * cdlam);
		if (fabs(z) > 1.) {
			if (fabs(z) > ONEEPS) F_ERROR
			else z = z < 0. ? -1. : 1.;
		} else
			z = acos(z);
		Av = Azba;
		xy.y = -rhoc;
	}
	if (z < 0.) F_ERROR;
	r = F * (t = pow(tan(.5 * z), n));
	if ((al = .5 * (R104 - z)) < 0.) F_ERROR;
	al = (t + pow(al, n)) / T;
	if (fabs(al) > 1.) {
		if (fabs(al) > ONEEPS) F_ERROR
		else al = al < 0. ? -1. : 1.;
	} else
		al = acos(al);
	if (fabs(t = n * (Av - Az)) < al)
		r /= cos(al + (tag ? t : -t));
	xy.x = r * sin(t);
	xy.y += (tag ? -r : r) * cos(t);
	if (P->noskew) {
		t = xy.x;
		xy.x = -xy.x * cAzc - xy.y * sAzc;
		xy.y = -xy.y * cAzc + t * sAzc;
	}
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double t, r, rp, rl, al, z, fAz, Az, s, c, Av;
	int neg, i;

	if (P->noskew) {
		t = xy.x;
		xy.x = -xy.x * cAzc + xy.y * sAzc;
		xy.y = -xy.y * cAzc - t * sAzc;
	}
	if( (neg = (xy.x < 0.)) ) {
		xy.y = rhoc - xy.y;
		s = S20;
		c = C20;
		Av = Azab;
	} else {
		xy.y += rhoc;
		s = S45;
		c = C45;
		Av = Azba;
	}
	rl = rp = r = hypot(xy.x, xy.y);
	fAz = fabs(Az = atan2(xy.x, xy.y));
	for (i = NITER; i ; --i) {
		z = 2. * atan(pow(r / F,1 / n));
		al = acos((pow(tan(.5 * z), n) +
		   pow(tan(.5 * (R104 - z)), n)) / T);
		if (fAz < al)
			r = rp * cos(al + (neg ? Az : -Az));
		if (fabs(rl - r) < EPS)
			break;
		rl = r;
	}
	if (! i) I_ERROR;
	Az = Av - Az / n;
	lp.phi = asin(s * cos(z) + c * sin(z) * cos(Az));
	lp.lam = atan2(sin(Az), c / tan(z) - s * cos(Az));
	if (neg)
		lp.lam -= R110;
	else
		lp.lam = lamB - lp.lam;
	return (lp);
}
FREEUP; if (P) pj_dalloc(P); }
ENTRY0(bipc)
	P->noskew = pj_param(P->params, "bns").i;
	P->inv = s_inverse;
	P->fwd = s_forward;
	P->es = 0.;
ENDENTRY(P)
