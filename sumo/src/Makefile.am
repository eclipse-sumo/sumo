bin_PROGRAMS = sumo sumo-netconvert sumo-router sumo-od2trips
sumo_SOURCES = sumo_main.cpp sumo_help.h
sumo_netconvert_SOURCES = netconvert_main.cpp netconvert_help.h
sumo_router_SOURCES = router_main.cpp router_help.h
sumo_od2trips_SOURCES = od2trips_main.cpp od2trips_help.h


sumo_LDFLAGS = -L$(XERCESLIBPATH) $(all_libraries)
sumo_LDADD   = ./netload/libnetload.a ./microsim/libmicrosim.a \
./utils/common/libcommon.a ./utils/convert/libconvert.a \
./utils/options/liboptions.a ./utils/xml/libxml.a \
./utils/sumoxml/libsumoxml.a ./sumo_only/libsumoonly.a \
./helpers/libhelpers.a -l$(XERCESLIB)

sumo_netconvert_LDFLAGS = -L$(XERCESLIBPATH) $(all_libraries)
sumo_netconvert_LDADD   = ./netbuild/libnetbuild.a \
./netimport/cell/libcellimport.a ./netimport/sumo/libsumoimport.a \
./netimport/visum/libvisumimport.a ./netimport/xml/libxmlimport.a \
./netimport/arcview/libarcviewimport.a \
./netimport/vissim/libvissimimport.a \
./netimport/vissim/typeloader/libvissimtypeload.a \
./netimport/vissim/tempstructs/libvissimtmpstr.a \
./utils/distribution/libdistribution.a \
./utils/importio/libimportio.a ./utils/geom/libgeom.a ./utils/gfx/libgfx.a \
./utils/sumoxml/libsumoxml.a ./utils/options/liboptions.a \
./utils/common/libcommon.a ./utils/convert/libconvert.a \
./utils/xml/libxml.a -l$(XERCESLIB)

sumo_router_LDFLAGS = -L$(XERCESLIBPATH) $(all_libraries)
sumo_router_LDADD   = ./router/librouter.a \
./utils/options/liboptions.a ./utils/convert/libconvert.a \
./utils/common/libcommon.a ./utils/xml/libxml.a \
./utils/router/librouter.a ./utils/importio/libimportio.a \
./utils/sumoxml/libsumoxml.a -l$(XERCESLIB)

sumo_od2trips_LDFLAGS = -L$(XERCESLIBPATH) $(all_libraries)
sumo_od2trips_LDADD   = ./od2trips/libod2trips.a \
./utils/options/liboptions.a ./utils/convert/libconvert.a \
./utils/common/libcommon.a ./utils/xml/libxml.a \
./utils/sumoxml/libsumoxml.a -l$(XERCESLIB)

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/sumo \
-I$(XERCESINCLUDE) -I$(XERCESINCLUDE_PRIOR_1_7) $(all_includes)

SUBDIRS = gui \
guinetload \
guisim \
helpers \
microsim \
netbuild \
netimport \
netload \
od2trips \
router \
sumo_only \
utils \

EXTRA_DIST = version.cpp

