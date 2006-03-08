#ifndef lint
static char GEODESIC_H_ID[] = "@(#)geodesic.h	4.3	95/08/19	GIE	REL";
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _IN_GEOD_SET
#  define GEOD_EXTERN extern
#else
#  define GEOD_EXTERN
#endif

GEOD_EXTERN struct geodesic {
	double	A;
	double	LAM1, PHI1, ALPHA12;
	double	LAM2, PHI2, ALPHA21;
	double	DIST;
	double	ONEF, FLAT, FLAT2, FLAT4, FLAT64;
	int	ELLIPSE;
} GEODESIC;

# define geod_a	GEODESIC.A
# define lam1	GEODESIC.LAM1
# define phi1	GEODESIC.PHI1
# define al12	GEODESIC.ALPHA12
# define lam2	GEODESIC.LAM2
# define phi2	GEODESIC.PHI2
# define al21	GEODESIC.ALPHA21
# define geod_S	GEODESIC.DIST
# define geod_f	GEODESIC.FLAT
# define onef	GEODESIC.ONEF
# define f2	GEODESIC.FLAT2
# define f4	GEODESIC.FLAT4
# define ff2	GEODESIC.FLAT4
# define f64	GEODESIC.FLAT64
# define ellipse GEODESIC.ELLIPSE


GEOD_EXTERN int n_alpha, n_S;
GEOD_EXTERN double to_meter, fr_meter, del_alpha;

void geod_set(int, char **);
void geod_for(void);
void geod_pre(void);
void geod_inv(void);

#ifdef __cplusplus
}
#endif
