/* Convert radian argument to DMS ascii format */
#ifndef lint
static const char SCCSID[]="@(#)rtodms.c	4.3	93/06/12	GIE	REL";
#endif
#include <projects.h>
#include <stdio.h>
#include <string.h>
/*
** RES is fractional second figures
** RES60 = 60 * RES
** CONV = 180 * 3600 * RES / PI (radians to RES seconds)
*/
	static double
RES = 1000.,
RES60 = 60000.,
CONV = 206264806.24709635515796003417;
	static char
format[50] = "%dd%d'%.3f\"%c";
	static int
dolong = 0;
	void
set_rtodms(int fract, int con_w) {
	int i;

	if (fract >= 0 && fract < 9 ) {
		RES = 1.;
		/* following not very elegant, but used infrequently */
		for (i = 0; i < fract; ++i)
			RES *= 10.;
		RES60 = RES * 60.;
		CONV = 180. * 3600. * RES / PI;
		if (! con_w)
			(void)sprintf(format,"%%dd%%d'%%.%df\"%%c", fract);
		else
			(void)sprintf(format,"%%dd%%02d'%%0%d.%df\"%%c",
				fract+2+(fract?1:0), fract);
		dolong = con_w;
	}
}
	char *
rtodms(char *s, double r, int pos, int neg) {
	int deg, min, sign;
	char *ss = s;
	double sec;

	if (r < 0) {
		r = -r;
		if  (!pos) { *ss++ = '-'; sign = 0; }
		else sign = neg;
	} else
		sign = pos;
	r = floor(r * CONV + .5);
	sec = fmod(r / RES, 60.);
	r = floor(r / RES60);
	min = fmod(r, 60.);
	deg = r / 60.;
	if (dolong)
		(void)sprintf(ss,format,deg,min,sec,sign);
	else if (sec) {
		char *p, *q;

		(void)sprintf(ss,format,deg,min,sec,sign);
		for (q = p = ss + strlen(ss) - (sign ? 3 : 2); *p == '0'; --p) ;
		if (*p != '.')
			++p;
		if (++q != p)
			(void)strcpy(p, q);
	} else if (min)
		(void)sprintf(ss,"%dd%d'%c",deg,min,sign);
	else
		(void)sprintf(ss,"%dd%c",deg, sign);
	return s;
}
