if WITH_GUI
GUI_APP = sumo-gui
if INTERNAL
NETEDIT_APP = netedit
endif
endif

if INTERNAL
MAROUTER_APP = marouter
endif

if TRACI
TRACI_TESTCLIENT_APP = TraCITestClient
endif

CLEANFILES = sumo sumoD meso mesoD \
sumo-gui sumo-guiD meso-gui meso-guiD \
dfrouter dfrouterD dfrouterInt dfrouterIntD \
duarouter duarouterD duarouterInt duarouterIntD \
jtrrouter jtrrouterD jtrrouterInt jtrrouterIntD \
netconvert netconvertD netconvertInt netconvertIntD \
netgenerate netgenerateD netgenerateInt netgenerateIntD \
od2trips od2tripsD od2tripsInt od2tripsIntD \
polyconvert polyconvertD polyconvertInt polyconvertIntD \
activitygen activitygenD activitygenInt activitygenIntD \
TraCITestClient TraCITestClientD TraCITestClientInt TraCITestClientIntD \
sumo-unittest sumo-unittestD \
netedit neteditD marouter marouterD

all: sumo dfrouter duarouter jtrrouter netconvert netgenerate od2trips polyconvert activitygen $(GUI_APP) $(NETEDIT_APP) $(MAROUTER_APP) $(TRACI_TESTCLIENT_APP)
sumo: ../src/sumo
	cp ../src/sumo ./`echo sumo | sed '$(transform)'`
	if [ ! -e sumo -a -e meso ]; then ln -s meso sumo; fi
	if [ ! -e sumoD -a -e mesoD ]; then ln -s mesoD sumoD; fi
dfrouter: ../src/dfrouter/dfrouter
	cp ../src/dfrouter/dfrouter ./`echo dfrouter | sed '$(transform)'`
	if [ ! -e dfrouter -a -e dfrouterInt ]; then ln -s dfrouterInt dfrouter; fi
	if [ ! -e dfrouterD -a -e dfrouterIntD ]; then ln -s dfrouterIntD dfrouterD; fi
duarouter: ../src/duarouter/duarouter
	cp ../src/duarouter/duarouter ./`echo duarouter | sed '$(transform)'`
	if [ ! -e duarouter -a -e duarouterInt ]; then ln -s duarouterInt duarouter; fi
	if [ ! -e duarouterD -a -e duarouterIntD ]; then ln -s duarouterIntD duarouterD; fi
jtrrouter: ../src/jtrrouter/jtrrouter
	cp ../src/jtrrouter/jtrrouter ./`echo jtrrouter | sed '$(transform)'`
	if [ ! -e jtrrouter -a -e jtrrouterInt ]; then ln -s jtrrouterInt jtrrouter; fi
	if [ ! -e jtrrouterD -a -e jtrrouterIntD ]; then ln -s jtrrouterIntD jtrrouterD; fi
netconvert: ../src/netconvert
	cp ../src/netconvert ./`echo netconvert | sed '$(transform)'`
	if [ ! -e netconvert -a -e netconvertInt ]; then ln -s netconvertInt netconvert; fi
	if [ ! -e netconvertD -a -e netconvertIntD ]; then ln -s netconvertIntD netconvertD; fi
netgenerate: ../src/netgen/netgenerate
	cp ../src/netgen/netgenerate ./`echo netgenerate | sed '$(transform)'`
	if [ ! -e netgenerate -a -e netgenerateInt ]; then ln -s netgenerateInt netgenerate; fi
	if [ ! -e netgenerateD -a -e netgenerateIntD ]; then ln -s netgenerateIntD netgenerateD; fi
od2trips: ../src/od2trips/od2trips
	cp ../src/od2trips/od2trips ./`echo od2trips | sed '$(transform)'`
	if [ ! -e od2trips -a -e od2tripsInt ]; then ln -s od2tripsInt od2trips; fi
	if [ ! -e od2tripsD -a -e od2tripsIntD ]; then ln -s od2tripsIntD od2tripsD; fi
polyconvert: ../src/polyconvert/polyconvert
	cp ../src/polyconvert/polyconvert ./`echo polyconvert | sed '$(transform)'`
	if [ ! -e polyconvert -a -e polyconvertInt ]; then ln -s polyconvertInt polyconvert; fi
	if [ ! -e polyconvertD -a -e polyconvertIntD ]; then ln -s polyconvertIntD polyconvertD; fi
activitygen: ../src/activitygen/activitygen
	cp ../src/activitygen/activitygen ./`echo activitygen | sed '$(transform)'`
	if [ ! -e activitygen -a -e activitygenInt ]; then ln -s activitygenInt activitygen; fi
	if [ ! -e activitygenD -a -e activitygenIntD ]; then ln -s activitygenIntD activitygenD; fi
$(GUI_APP): ../src/$(GUI_APP)
	cp ../src/$(GUI_APP) ./`echo $(GUI_APP) | sed '$(transform)'`
	if [ ! -e $(GUI_APP) -a -e meso-gui ]; then ln -s meso-gui $(GUI_APP); fi
	if [ ! -e $(GUI_APP)D -a -e meso-guiD ]; then ln -s meso-guiD $(GUI_APP)D; fi
$(NETEDIT_APP):	../src/netedit/$(NETEDIT_APP)
	cp ../src/netedit/$(NETEDIT_APP) ./`echo $(NETEDIT_APP) | sed '$(transform)'`
$(MAROUTER_APP): ../src/marouter/$(MAROUTER_APP)
	cp ../src/marouter/$(MAROUTER_APP) ./`echo $(MAROUTER_APP) | sed '$(transform)'`
$(TRACI_TESTCLIENT_APP): ../src/traci_testclient/$(TRACI_TESTCLIENT_APP)
	cp ../src/traci_testclient/$(TRACI_TESTCLIENT_APP) ./`echo $(TRACI_TESTCLIENT_APP) | sed '$(transform)'`
	if [ ! -e $(TRACI_TESTCLIENT_APP) -a -e $(TRACI_TESTCLIENT_APP)Int ]; then ln -s $(TRACI_TESTCLIENT_APP)Int $(TRACI_TESTCLIENT_APP); fi
	if [ ! -e $(TRACI_TESTCLIENT_APP)D -a -e $(TRACI_TESTCLIENT_APP)IntD ]; then ln -s $(TRACI_TESTCLIENT_APP)IntD $(TRACI_TESTCLIENT_APP)D; fi
