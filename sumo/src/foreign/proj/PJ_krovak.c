/******************************************************************************
 * $Id$
 *
 * Project:  PROJ.4
 * Purpose:  Implementation of the krovak (Krovak) projection.
 *           Definition: http://www.ihsenergy.com/epsg/guid7.html#1.4.3
 * Author:   Thomas Flemming, tf@ttqv.com
 *
 ******************************************************************************
 * Copyright (c) 2001, Thomas Flemming, tf@ttqv.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************
 *
 * $Log$
 * Revision 1.1  2006/03/08 13:02:28  dkrajzew
 * some further work on converting geo-coordinates
 *
 * Revision 1.4  2002/12/15 22:31:04  warmerda
 * handle lon_0, k, and prime meridian properly
 *
 * Revision 1.3  2002/12/15 00:13:30  warmerda
 * lat_0 may now be set by user, but still defaults to 49d30N
 *
 * Revision 1.2  2002/12/14 19:35:21  warmerda
 * updated headers
 *
 */

#define PROJ_PARMS__ \
	double	C_x;
#define PJ_LIB__

#include <projects.h>
#include <string.h>
#include <stdio.h>

PJ_CVSID("$Id$");

PROJ_HEAD(krovak, "Krovak") "\n\tPCyl., Sph.";

/**
   NOTES: According to EPSG the full Krovak projection method should have
          the following parameters.  Within PROJ.4 the azimuth, and pseudo
          standard parallel are hardcoded in the algorithm and can't be
          altered from outside.  The others all have defaults to match the
          common usage with Krovak projection.

  lat_0 = latitude of centre of the projection

  lon_0 = longitude of centre of the projection

  ** = azimuth (true) of the centre line passing through the centre of the projection

  ** = latitude of pseudo standard parallel

  k  = scale factor on the pseudo standard parallel

  x_0 = False Easting of the centre of the projection at the apex of the cone

  y_0 = False Northing of the centre of the projection at the apex of the cone

 **/



FORWARD(s_forward); /* spheroid */
/* calculate xy from lat/lon */

	char errmess[255];
	char tmp[16];

/* Constants, identical to inverse transform function */
	double s45, s90, e2, e, alfa, uq, u0, g, k, k1, n0, ro0, ad, a, s0, n;
	double gfi, u, fi0, lon17, lamdd, deltav, s, d, eps, ro;


	s45 = 0.785398163397448;    /* 45° */
	s90 = 2 * s45;
	fi0 = P->phi0;    /* Latitude of projection centre 49° 30' */

   /* Ellipsoid is used as Parameter in for.c and inv.c, therefore a must
      be set to 1 here.
      Ellipsoid Bessel 1841 a = 6377397.155m 1/f = 299.1528128,
      e2=0.006674372230614;
   */
	a =  1; /* 6377397.155; */
	/* e2 = P->es;*/       /* 0.006674372230614; */
	e2 = 0.006674372230614;
	e = sqrt(e2);

	alfa = sqrt(1. + (e2 * pow(cos(fi0), 4)) / (1. - e2));

	uq = 1.04216856380474;      /* DU(2, 59, 42, 42.69689) */
	u0 = asin(sin(fi0) / alfa);
	g = pow(   (1. + e * sin(fi0)) / (1. - e * sin(fi0)) , alfa * e / 2.  );

	k = tan( u0 / 2. + s45) / pow  (tan(fi0 / 2. + s45) , alfa) * g;

	k1 = P->k0;
	n0 = a * sqrt(1. - e2) / (1. - e2 * pow(sin(fi0), 2));
	s0 = 1.37008346281555;       /* Latitude of pseudo standard parallel 78° 30'00" N */
	n = sin(s0);
	ro0 = k1 * n0 / tan(s0);
	ad = s90 - uq;

/* Transformation */

	gfi =pow ( ((1. + e * sin(lp.phi)) /
               (1. - e * sin(lp.phi))) , (alfa * e / 2.));

	u= 2. * (atan(k * pow( tan(lp.phi / 2. + s45), alfa) / gfi)-s45);

	deltav = - lp.lam * alfa;

	s = asin(cos(ad) * sin(u) + sin(ad) * cos(u) * cos(deltav));
	d = asin(cos(u) * sin(deltav) / cos(s));
	eps = n * d;
	ro = ro0 * pow(tan(s0 / 2. + s45) , n) / pow(tan(s / 2. + s45) , n)   ;

   /* x and y are reverted! */
	xy.y = ro * cos(eps) / a;
	xy.x = ro * sin(eps) / a;

#ifdef DEBUG
	strcpy(errmess,"a: ");
	strcpy(tmp,"        ");
	ltoa((long)(a*1000000000),tmp,10);
	strcat(errmess,tmp);
	strcat(errmess,"e2: ");
	strcpy(tmp,"        ");
	ltoa((long)(e2*1000000000),tmp,10);
	strcat(errmess,tmp);

	MessageBox(NULL, errmess, NULL, 0);
#endif


	return (xy);
}



INVERSE(s_inverse); /* spheroid */
	/* calculate lat/lon from xy */

/* Constants, identisch wie in der Umkehrfunktion */
	double s45, s90, fi0, e2, e, alfa, uq, u0, g, k, k1, n0, ro0, ad, a, s0, n;
	double u, l24, lamdd, deltav, s, d, eps, ro, fi1, xy0, lon17;
	int ok;

	s45 = 0.785398163397448;    /* 45° */
	s90 = 2 * s45;
	fi0 = P->phi0;    /* Latitude of projection centre 49° 30' */


   /* Ellipsoid is used as Parameter in for.c and inv.c, therefore a must
      be set to 1 here.
      Ellipsoid Bessel 1841 a = 6377397.155m 1/f = 299.1528128,
      e2=0.006674372230614;
   */
	a = 1; /* 6377397.155; */
	/* e2 = P->es; */      /* 0.006674372230614; */
	e2 = 0.006674372230614;
	e = sqrt(e2);

	alfa = sqrt(1. + (e2 * pow(cos(fi0), 4)) / (1. - e2));
	uq = 1.04216856380474;      /* DU(2, 59, 42, 42.69689) */
	u0 = asin(sin(fi0) / alfa);
	g = pow(   (1. + e * sin(fi0)) / (1. - e * sin(fi0)) , alfa * e / 2.  );

	k = tan( u0 / 2. + s45) / pow  (tan(fi0 / 2. + s45) , alfa) * g;

	k1 = P->k0;
	n0 = a * sqrt(1. - e2) / (1. - e2 * pow(sin(fi0), 2));
	s0 = 1.37008346281555;       /* Latitude of pseudo standard parallel 78° 30'00" N */
	n = sin(s0);
	ro0 = k1 * n0 / tan(s0);
	ad = s90 - uq;


/* Transformation */
   /* revert y, x*/
	xy0=xy.x;
	xy.x=xy.y;
	xy.y=xy0;

	ro = sqrt(xy.x * xy.x + xy.y * xy.y);
	eps = atan2(xy.y, xy.x);
	d = eps / sin(s0);
	s = 2. * (atan(  pow(ro0 / ro, 1. / n) * tan(s0 / 2. + s45)) - s45);

	u = asin(cos(ad) * sin(s) - sin(ad) * cos(s) * cos(d));
	deltav = asin(cos(s) * sin(d) / cos(u));

	lp.lam = P->lam0 - deltav / alfa;

/* ITERATION FOR lp.phi */
   fi1 = u;

   ok = 0;
   do
   {
   	lp.phi = 2. * ( atan( pow( k, -1. / alfa)  *
                            pow( tan(u / 2. + s45) , 1. / alfa)  *
                            pow( (1. + e * sin(fi1)) / (1. - e * sin(fi1)) , e / 2.)
                           )  - s45);

      if (fabs(fi1 - lp.phi) < 0.000000000000001) ok=1;
      fi1 = lp.phi;

   }
   while (ok==0);

   lp.lam -= P->lam0;

   return (lp);
}

FREEUP; if (P) pj_dalloc(P); }

ENTRY0(krovak)
	double ts;
	/* read some Parameters,
	 * here Latitude Truescale */

	ts = pj_param(P->params, "rlat_ts").f;
	P->C_x = ts;

	/* we want Bessel as fixed ellipsoid */
	P->a = 6377397.155;
	P->e = sqrt(P->es = 0.006674372230614);

        /* if latitude of projection center is not set, use 49d30'N */
	if (!pj_param(P->params, "tlat_0").i)
            P->phi0 = 0.863937979737193;

        /* if center long is not set use 42d30'E of Ferro - 17d40' for Ferro */
        /* that will correspond to using longitudes relative to greenwich    */
        /* as input and output, instead of lat/long relative to Ferro */
	if (!pj_param(P->params, "tlon_0").i)
            P->lam0 = 0.7417649320975901 - 0.308341501185665;
;

        /* if scale not set default to 0.9999 */
	if (!pj_param(P->params, "tk").i)
            P->k0 = 0.9999;

	/* always the same */
        P->inv = s_inverse;
	P->fwd = s_forward;

ENDENTRY(P)

