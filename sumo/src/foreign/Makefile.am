if CHECK_MEMORY_LEAKS
MEMDIRS = nvwa
endif

if WITH_GUI
GUIDIRS = polyfonts
endif

if TRACI
TRACI_TESTCLIENTDIRS = traci_testclient
endif

EXTRA_DIST = additional.txt

SUBDIRS = mersenne tcpip $(MEMDIRS) $(GUIDIRS) $(TRACI_TESTCLIENTDIRS)
