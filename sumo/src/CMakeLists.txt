message("entering src")

include_directories(${CMAKE_SOURCE_DIR}/src)

#add_subdirectory(activitygen)
#add_subdirectory(dfrouter)
#add_subdirectory(duarouter)
add_subdirectory(foreign)
add_subdirectory(gui)
add_subdirectory(guinetload)
add_subdirectory(guisim)
#add_subdirectory(jtrrouter)
#add_subdirectory(marouter)
add_subdirectory(mesogui)
add_subdirectory(mesosim)
add_subdirectory(microsim)
#add_subdirectory(netbuild)
#add_subdirectory(netedit)
#add_subdirectory(netgen)
#add_subdirectory(netimport)
add_subdirectory(netload)
#add_subdirectory(netwrite)
#add_subdirectory(od)
add_subdirectory(osgview)
#add_subdirectory(polyconvert)
#add_subdirectory(router)
#add_subdirectory(tools)
add_subdirectory(traci-server)
#add_subdirectory(traci_testclient)
add_subdirectory(utils)



if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(sumoexec 
		sumo_main.cpp
		microsim/MSMoveReminder.cpp microsim/MSMoveReminder.h)
	set(sumoguiexec
		guisim_main.cpp
		microsim/MSMoveReminder.cpp microsim/MSMoveReminder.h
		gui/GUIManipulator.h gui/GUIManipulator.cpp
		gui/GUITLLogicPhasesTrackerWindow.h gui/GUITLLogicPhasesTrackerWindow.cpp
		mesosim/MEVehicleControl.h mesosim/MEVehicleControl.cpp)
else ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
 	set(sumoexec 
                sumo_main.cpp)
	set(sumoguiexec
		guisim_main.cpp)
endif()
	

add_executable( sumo ${sumoexec})
target_link_libraries(sumo netload ms mscfmodels mslcmodels msdevices msoutput mspedestrians mstrigger msactions
        mstraffic_lights mesosim utgeom utvehicle utshapes utemissions traci-server uttraci utoptions utxml
	utcommon utimportio utiodevices foreignphemlight foreigntcpip xerces-c ${FOX_LIBRARIES} proj ${GRPC_LIBS} ${PYTHON_LIBRARIES})

add_executable(sumo-gui ${sumoguiexec})
target_link_libraries(sumo-gui gui guinetload guisim guidialogs utguiwindows utguiglobjects utguidiv utguisettings
        utguiimages utguitracker utfoxtools osgview netload ms mscfmodels mslcmodels msdevices msoutput mspedestrians
        mstrigger msactions mstraffic_lights mesosim mesogui utgeom utvehicle utshapes utemissions traci-server
        uttraci utoptions utxml utcommon utimportio utiodevices foreignphemlight foreigntcpip xerces-c ${FOX_LIBRARY}
	proj ${FOX_LIBRARIES} gdal ${FFMPEG_LIBRARIES} gl2ps foreignpolyfonts ${GRPC_LIBS} ${PYTHON_LIBRARIES})
