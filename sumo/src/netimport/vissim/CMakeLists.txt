message("entering vissim")

add_subdirectory(tempstructs)
add_subdirectory(typeloader)


set(vissim_files
        NIImporter_Vissim.cpp NIImporter_Vissim.h
       NIVissimElements.h)

add_library(vi STATIC ${vissim_files})
