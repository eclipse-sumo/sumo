XERCES_LIBS = -l$(LIB_XERCES)

if CHECK_MEMORY_LEAKS
MEM_LIBS  = ../foreign/nvwa/libnvwa.a
endif

COMMON_LIBS = ../utils/options/liboptions.a \
../utils/common/libcommon.a \
../utils/xml/libxml.a \
../utils/importio/libimportio.a \
../utils/iodevices/libiodevices.a \
../foreign/tcpip/libtcpip.a \
$(MEM_LIBS) \
$(XERCES_LIBS)

bin_PROGRAMS = activitygen

activitygen_SOURCES = AGMain.cpp \
../duarouter/RODUAEdgeBuilder.cpp ../duarouter/RODUAEdgeBuilder.h \
../duarouter/RODUAFrame.cpp ../duarouter/RODUAFrame.h \
AGActivityGen.cpp AGActivityGen.h \
AGActivityGenHandler.cpp AGActivityGenHandler.h \
AGActivityTripWriter.cpp AGActivityTripWriter.h \
city/AGTime.cpp city/AGTime.h \
city/AGCity.cpp city/AGCity.h \
city/AGStreet.cpp city/AGStreet.h \
city/AGPosition.cpp city/AGPosition.h \
city/AGSchool.cpp city/AGSchool.h \
city/AGBusLine.cpp city/AGBusLine.h \
city/AGBus.cpp city/AGBus.h \
city/AGHousehold.cpp city/AGHousehold.h \
city/AGWorkPosition.cpp city/AGWorkPosition.h \
city/AGPerson.cpp city/AGPerson.h \
city/AGAdult.cpp city/AGAdult.h \
city/AGChild.cpp city/AGChild.h \
city/AGDataAndStatistics.cpp city/AGDataAndStatistics.h \
city/AGCar.cpp city/AGCar.h \
activities/AGActivities.cpp activities/AGActivities.h \
activities/AGTrip.cpp activities/AGTrip.h \
activities/AGActivity.cpp activities/AGActivity.h \
activities/AGWorkAndSchool.cpp activities/AGWorkAndSchool.h \
activities/AGFreeTime.cpp activities/AGFreeTime.h

activitygen_LDFLAGS = $(XERCES_LDFLAGS)
activitygen_LDADD =  ../router/librouter.a \
$(COMMON_LIBS)
