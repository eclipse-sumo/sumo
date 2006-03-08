/* reduce argument to range +/- PI */
#ifndef lint
static const char SCCSID[]="@(#)adjlon.c	4.3	93/06/12	GIE	REL";
#endif
#include <math.h>
#include <projects.h>

#define SPI     3.14159265359
#define TWOPI   6.2831853071795864769
#define ONEPI   3.14159265358979323846

double adjlon (double lon) {
    if (fabs(lon) <= SPI) return( lon );
    lon += ONEPI;  /* adjust to 0..2pi rad */
    lon -= TWOPI * floor(lon / TWOPI); /* remove integral # of 'revolutions'*/
    lon -= ONEPI;  /* adjust back to -pi..pi rad */
    return( lon );
}
