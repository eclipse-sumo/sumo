/*!
* \file jniproj.c
*
* \brief
* functions used by the java/jni wrappers of jproj4
*
*
* $Id$
*
* \author Antonello Andrea
* \date   Wed Oct 20 23:10:24 CEST 2004
*/
#include "proj_config.h"

#ifdef JNI_ENABLED

#include "projects.h"
#include "org_proj4_Projections.h"
#include <jni.h>

#define arraysize 300

/*!
 * \brief
 * executes reprojection
 *
 * JNI informations:
 * Class:     org_proj4_Projections
 * Method:    transform
 * Signature: ([D[D[DLjava/lang/String;Ljava/lang/String;JI)V
 *
 *
 * \param env - parameter used by jni (see JNI specification)
 * \param parent - parameter used by jni (see JNI specification)
 * \param firstcoord - array of x coordinates
 * \param secondcoord - array of y coordinates
 * \param values - array of z coordinates
 * \param src - definition of the source projection
 * \param dest - definition of the destination projection
 * \param pcount
 * \param poffset
*/
JNIEXPORT void JNICALL Java_org_proj4_Projections_transform
  (JNIEnv * env, jobject parent, jdoubleArray firstcoord, jdoubleArray secondcoord, jdoubleArray values, jstring src, jstring dest, jlong pcount, jint poffset)
{
	int i;
	projPJ src_pj, dst_pj;
	char * srcproj_def = (char *) (*env)->GetStringUTFChars (env, src, 0);
	char * destproj_def = (char *) (*env)->GetStringUTFChars (env, dest, 0);

	if (!(src_pj = pj_init_plus(srcproj_def)))
		exit(1);
	if (!(dst_pj = pj_init_plus(destproj_def)))
		exit(1);

	double *xcoord = (* env)-> GetDoubleArrayElements(env, firstcoord, NULL);
	double *ycoord = (* env) -> GetDoubleArrayElements(env, secondcoord, NULL);
	double *zcoord = (* env) -> GetDoubleArrayElements(env, values, NULL);

	jint sizeofdata = (*env)->GetArrayLength(env, firstcoord);
	for(i = 0;i<sizeofdata;i++)
	{
		pj_transform( src_pj, dst_pj, pcount,poffset, xcoord, ycoord, zcoord);
		xcoord++;
		ycoord++;
		zcoord++;
	}
	xcoord = xcoord - sizeofdata;
	ycoord = ycoord - sizeofdata;
	zcoord = zcoord - sizeofdata;

	(* env)->ReleaseDoubleArrayElements(env,firstcoord,(jdouble *) xcoord,JNI_COMMIT);
	(* env)->ReleaseDoubleArrayElements(env,secondcoord,(jdouble *) ycoord,JNI_COMMIT);
	(* env)->ReleaseDoubleArrayElements(env,values,(jdouble *) zcoord,JNI_COMMIT);

	pj_free( src_pj );
	pj_free( dst_pj );
}

/*!
 * \brief
 * retrieves projection parameters
 *
 * JNI informations:
 * Class:     org_proj4_Projections
 * Method:    getProjInfo
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 *
 *
 * \param env - parameter used by jni (see JNI specification)
 * \param parent - parameter used by jni (see JNI specification)
 * \param projdefinition - definition of the projection
*/
JNIEXPORT jstring JNICALL Java_org_proj4_Projections_getProjInfo
  (JNIEnv * env, jobject parent, jstring projdefinition)
{
	PJ *pj;
	char * pjdesc;
	char info[arraysize];

	char * proj_def = (char *) (*env)->GetStringUTFChars (env, projdefinition, 0);

	if (!(pj = pj_init_plus(proj_def)))
		exit(1);

	// put together all the info of the projection and free the pointer to pjdesc
	pjdesc = pj_get_def(pj, 0);
	strcpy(info,pjdesc);
	pj_dalloc(pjdesc);

	return (*env)->NewStringUTF(env,info);
}


/*!
 * \brief
 * retrieves ellipsoid parameters
 *
 * JNI informations:
 * Class:     org_proj4_Projections
 * Method:    getEllipsInfo
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 *
 *
 * \param env - parameter used by jni (see JNI specification)
 * \param parent - parameter used by jni (see JNI specification)
 * \param projdefinition - definition of the projection
*/
JNIEXPORT jstring JNICALL Java_org_proj4_Projections_getEllipsInfo
  (JNIEnv * env, jobject parent, jstring projdefinition)
{
	PJ *pj;
	char * pjdesc;
	char ellipseinfo[arraysize];
	char temp[50];

	char * proj_def = (char *) (*env)->GetStringUTFChars (env, projdefinition, 0);

	if (!(pj = pj_init_plus(proj_def)))
		exit(1);

	// put together all the info of the ellipsoid
/* 	sprintf(temp,"name: %s;", pj->descr); */
	sprintf(temp,"name: not available;");
	strcpy(ellipseinfo,temp);
	sprintf(temp,"a: %lf;", pj->a);
	strcat(ellipseinfo,temp);
	sprintf(temp,"e: %lf;", pj->e);
	strcat(ellipseinfo,temp);
	sprintf(temp,"es: %lf;", pj->es);
	strcat(ellipseinfo,temp);
	sprintf(temp,"ra: %lf;", pj->ra);
	strcat(ellipseinfo,temp);
	sprintf(temp,"one_es: %lf;", pj->one_es);
	strcat(ellipseinfo,temp);
	sprintf(temp,"rone_es: %lf;", pj->rone_es);
	strcat(ellipseinfo,temp);
	sprintf(temp,"lam0: %lf;", pj->lam0);
	strcat(ellipseinfo,temp);
	sprintf(temp,"phi0: %lf;", pj->phi0);
	strcat(ellipseinfo,temp);
	sprintf(temp,"x0: %lf;", pj->x0);
	strcat(ellipseinfo,temp);
	sprintf(temp,"y0: %lf;", pj->y0);
	strcat(ellipseinfo,temp);
	sprintf(temp,"k0: %lf;", pj->k0);
	strcat(ellipseinfo,temp);
	sprintf(temp,"to_meter: %lf;", pj->to_meter);
	strcat(ellipseinfo,temp);
	sprintf(temp,"fr_meter: %lf;", pj->fr_meter);
	strcat(ellipseinfo,temp);

	return (*env)->NewStringUTF(env,ellipseinfo);
}

#endif
