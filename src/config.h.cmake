/* Reporting string for enabled options */
#define HAVE_ENABLED "@ENABLED_FEATURES@"

/* defined if ffmpeg is available */
#cmakedefine HAVE_FFMPEG

/* defined if FOX is available */
#cmakedefine HAVE_FOX

/* defined if GDAL is available */
#cmakedefine HAVE_GDAL

/* defined if GL2PS is available */
#cmakedefine HAVE_GL2PS

/* defined if osg is available */
#cmakedefine HAVE_OSG

/* defined if PROJ is available */
#cmakedefine HAVE_PROJ

/* defined if python is available */
#cmakedefine HAVE_PYTHON

/* Define if auto-generated version.h should be used. */
#define HAVE_VERSION_H
#ifndef HAVE_VERSION_H
/* Define if auto-generated version.h is unavailable. */
#define VERSION_STRING "0.29.0"
#endif

/* Define to 1 in order to disable TraCI Server. */
#undef NO_TRACI

/* defines the epsilon to use on general floating point comparison */
#define NUMERICAL_EPS 0.001

/* defines the epsilon to use on position comparison */
#define POSITION_EPS 0.1

/* Define length for Xerces 3. */
#define XERCES3_SIZE_t XMLSize_t
