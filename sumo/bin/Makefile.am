if WITH_GUI
GUI_APPS = sumo-gui
if INTERNAL
NETEDIT_APP = netedit
MAROUTER_APP = marouter
endif
endif

if TRACI
TRACI_TESTCLIENT_APPS = TraCITestClient
endif

DISTCLEANFILES = sumo sumoD dfrouter dfrouterD duarouter duarouterD jtrrouter jtrrouterD netconvert netconvertD netgen netgenD od2trips od2tripsD polyconvert polyconvertD activitygen activitygenD sumo-unittest sumo-unittestD sumo-gui sumo-guiD TraCITestClient TraCITestClientD meso mesoD meso-gui meso-guiD netedit neteditD marouter marouterD

all: sumo dfrouter duarouter jtrrouter netconvert netgen od2trips polyconvert activitygen $(GUI_APPS) $(NETEDIT_APP) $(MAROUTER_APP) $(TRACI_TESTCLIENT_APPS)
sumo: ../src/sumo
	cp ../src/sumo ./`echo sumo | sed '$(transform)'`
	if [ ! -e sumo -a -e meso ]; then ln -s meso sumo; fi
	if [ ! -e sumoD -a -e mesoD ]; then ln -s mesoD sumoD; fi
dfrouter: ../src/dfrouter/dfrouter
	cp ../src/dfrouter/dfrouter ./`echo dfrouter | sed '$(transform)'`
	if [ ! -e dfrouter -a -e dfrouterInt ]; then ln -s dfrouterInt dfrouter; fi
	if [ ! -e dfrouter -a -e dfrouterInt ]; then ln -s dfrouterInt dfrouter; fi
duarouter: ../src/duarouter/duarouter
	cp ../src/duarouter/duarouter ./`echo duarouter | sed '$(transform)'`
	if [ ! -e duarouter -a -e duarouterInt ]; then ln -s duarouterInt duarouter; fi
	if [ ! -e duarouter -a -e duarouterInt ]; then ln -s duarouterInt duarouter; fi
jtrrouter: ../src/jtrrouter/jtrrouter
	cp ../src/jtrrouter/jtrrouter ./`echo jtrrouter | sed '$(transform)'`
	if [ ! -e jtrrouter -a -e jtrrouterInt ]; then ln -s jtrrouterInt jtrrouter; fi
	if [ ! -e jtrrouter -a -e jtrrouterInt ]; then ln -s jtrrouterInt jtrrouter; fi
netconvert: ../src/netconvert
	cp ../src/netconvert ./`echo netconvert | sed '$(transform)'`
	if [ ! -e netconvert -a -e netconvertInt ]; then ln -s netconvertInt netconvert; fi
	if [ ! -e netconvert -a -e netconvertInt ]; then ln -s netconvertInt netconvert; fi
netgen: ../src/netgen/netgen
	cp ../src/netgen/netgen ./`echo netgen | sed '$(transform)'`
	if [ ! -e netgen -a -e netgenInt ]; then ln -s netgenInt netgen; fi
	if [ ! -e netgen -a -e netgenInt ]; then ln -s netgenInt netgen; fi
od2trips: ../src/od2trips/od2trips
	cp ../src/od2trips/od2trips ./`echo od2trips | sed '$(transform)'`
	if [ ! -e od2trips -a -e od2tripsInt ]; then ln -s od2tripsInt od2trips; fi
	if [ ! -e od2trips -a -e od2tripsInt ]; then ln -s od2tripsInt od2trips; fi
polyconvert: ../src/polyconvert/polyconvert
	cp ../src/polyconvert/polyconvert ./`echo polyconvert | sed '$(transform)'`
	if [ ! -e polyconvert -a -e polyconvertInt ]; then ln -s polyconvertInt polyconvert; fi
	if [ ! -e polyconvert -a -e polyconvertInt ]; then ln -s polyconvertInt polyconvert; fi
activitygen: ../src/activitygen/activitygen
	cp ../src/activitygen/activitygen ./`echo activitygen | sed '$(transform)'`
	if [ ! -e activitygen -a -e activitygenInt ]; then ln -s activitygenInt activitygen; fi
	if [ ! -e activitygen -a -e activitygenInt ]; then ln -s activitygenInt activitygen; fi
$(GUI_APPS): ../src/$(GUI_APPS)
	cp ../src/$(GUI_APPS) ./`echo $(GUI_APPS) | sed '$(transform)'`
	if [ ! -e sumo-gui -a -e meso-gui ]; then ln -s meso-gui sumo-gui; fi
	if [ ! -e sumo-guiD -a -e meso-guiD ]; then ln -s meso-guiD sumo-guiD; fi
$(NETEDIT_APP):	../src/netedit/$(NETEDIT_APP)
	cp ../src/netedit/$(NETEDIT_APP) ./`echo $(NETEDIT_APP) | sed '$(transform)'`
$(MAROUTER_APP): ../src/marouter/$(MAROUTER_APP)
	cp ../src/marouter/$(MAROUTER_APP) ./`echo $(MAROUTER_APP) | sed '$(transform)'`
$(TRACI_TESTCLIENT_APPS): ../src/traci_testclient/$(TRACI_TESTCLIENT_APPS)
	cp ../src/traci_testclient/$(TRACI_TESTCLIENT_APPS) ./`echo $(TRACI_TESTCLIENT_APPS) | sed '$(transform)'`
