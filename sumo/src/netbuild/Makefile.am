noinst_LIBRARIES = libnetbuild.a

libnetbuild_a_SOURCES = NBCapacity2Lanes.h NBConnection.cpp NBConnection.h \
NBConnectionDefs.h NBCont.h NBContHelper.cpp NBContHelper.h \
NBDistribution.cpp NBDistribution.h NBDistrict.cpp NBDistrict.h \
NBDistrictCont.cpp NBDistrictCont.h NBEdge.cpp NBEdge.h \
NBEdgeCont.cpp NBEdgeCont.h NBHelpers.cpp NBHelpers.h \
NBJunctionLogicCont.cpp NBJunctionLogicCont.h \
NBJunctionTypesMatrix.cpp NBJunctionTypesMatrix.h \
NBLinkCliqueContainer.cpp NBLinkCliqueContainer.h \
NBLinkPossibilityMatrix.h \
NBLogicKeyBuilder.cpp NBLogicKeyBuilder.h \
NBRequest.cpp NBRequest.h \
NBRequestEdgeLinkIterator.cpp NBRequestEdgeLinkIterator.h \
NBTrafficLightDefinition.cpp NBTrafficLightDefinition.h \
NBTrafficLightLogic.cpp NBTrafficLightLogic.h \
NBTrafficLightLogicCont.cpp NBTrafficLightLogicCont.h \
NBTrafficLightLogicVector.cpp NBTrafficLightLogicVector.h \
NBTrafficLightPhases.cpp NBTrafficLightPhases.h \
NBType.cpp NBType.h NBTypeCont.cpp NBTypeCont.h NLLoadFilter.h \
NBOwnTLDef.cpp NBOwnTLDef.h \
NBLoadedTLDef.cpp NBLoadedTLDef.h \
NBNetBuilder.cpp NBNetBuilder.h \
NBJoinedEdgesMap.cpp NBJoinedEdgesMap.h

CFRONT_FILES =

SUBDIRS = nodes

EXTRA_DIST = $(CFRONT_FILES) NBMMLDirections.h

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src \
-I$(XERCES_INCLUDES) -I$(XERCES_INCLUDES_PRIOR_1_7)

