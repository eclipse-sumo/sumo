# Microsoft Developer Studio Project File - Name="x_libforeign_proj" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=x_libforeign_proj - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "x_libforeign_proj.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "x_libforeign_proj.mak" CFG="x_libforeign_proj - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "x_libforeign_proj - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "x_libforeign_proj - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "x_libforeign_proj - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\src" /I "..\..\..\src\foreign\proj" /D "ACE_HAS_DLL" /D "HAVE_CONFIG_H" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "x_libforeign_proj - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\src" /I "..\..\..\src\foreign\proj" /D "ACE_HAS_DLL" /D "HAVE_CONFIG_H" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "x_libforeign_proj - Win32 Release"
# Name "x_libforeign_proj - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\aasincos.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\adjlon.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\bch2bps.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\bchgen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\biveval.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\dmstor.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\emess.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\gen_cheb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\geocent.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\geod_for.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\geod_inv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\geod_set.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\mk_cheby.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\nad_cvt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\nad_init.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\nad_intr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\p_series.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_aea.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_aeqd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_airy.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_aitoff.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_apply_gridshift.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_august.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_auth.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_bacon.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_bipc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_boggs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_bonne.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_cass.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_cc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_cea.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_chamb.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_collg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_crast.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_datum_set.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_datums.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_denoy.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_deriv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_eck1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_eck2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_eck3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_eck4.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_eck5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_ell_set.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_ellps.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_eqc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_eqdc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_errno.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_factors.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_fahey.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_fouc_s.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_fwd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_gall.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_gauss.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_geocent.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_geos.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_gins8.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_gn_sinu.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_gnom.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_goode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_gridinfo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_gridlist.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_hammer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_hatano.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_imw_p.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_init.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_inv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_krovak.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_labrd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_laea.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_lagrng.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_larr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_lask.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_latlong.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_lcc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_lcca.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_loxim.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_lsat.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_malloc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_mbt_fps.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_mbtfpp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_mbtfpq.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_merc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_mill.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_mlfn.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_mod_ster.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_moll.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_mpoly.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_msfn.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_nell.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_nell_h.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_nocol.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_nsper.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_nzmg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_ob_tran.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_ocea.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_oea.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_omerc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_open_lib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_ortho.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_param.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_phi2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_poly.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_pr_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_putp2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_putp3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_putp4p.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_putp5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_putp6.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_qsfn.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_release.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_robin.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_rpoly.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_sconics.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_somerc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_stere.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_sterea.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_strerrno.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_sts.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_tcc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_tcea.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_tmerc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_tpeqd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_transform.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_tsfn.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_units.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_urm5.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_urmfps.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_utils.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_vandg.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_vandg2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_vandg4.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_wag2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_wag3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_wag7.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_wink1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\PJ_wink2.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_zpoly1.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\rtodms.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\vector1.c
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\emess.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\geocent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\geodesic.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\nad_list.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\org_proj4_Projections.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\pj_list.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\proj_api.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\foreign\proj\projects.h
# End Source File
# End Group
# End Target
# End Project
